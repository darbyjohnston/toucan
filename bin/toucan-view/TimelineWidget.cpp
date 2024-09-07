// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include "DocumentsModel.h"
#include "PlaybackModel.h"

#include "App.h"

#include <dtk/core/Random.h>
#include <dtk/core/RenderUtil.h>

namespace toucan
{
    void IItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::TimeRange& timeRange,
        const std::string& name,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, name, parent);
        _timeRange = timeRange;
    }

    IItem::~IItem()
    {}

    void IItem::setScale(double value)
    {
        if (value == _scale)
            return;
        _scale = value;
        for (const auto& child : getChildren())
        {
            if (auto item = std::dynamic_pointer_cast<IItem>(child))
            {
                item->setScale(value);
            }
        }
        _setSizeUpdate();
    }

    void ClipItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& clip,
        const std::shared_ptr<IWidget>& parent)
    {
        auto opt = clip->trimmed_range_in_parent();
        IItem::_init(
            context,
            opt.has_value() ? opt.value() : OTIO_NS::TimeRange(),
            "toucan::ClipItem",
            parent);

        _text = clip->name();
        setTooltip(_text);
    }
    
    ClipItem::~ClipItem()
    {}

    std::shared_ptr<ClipItem> ClipItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& clip,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<ClipItem>();
        out->_init(context, clip, parent);
        return out;
    }

    void ClipItem::setColor(const dtk::Color4F& value)
    {
        if (value == _color)
            return;
        _color = value;
        _setDrawUpdate();
    }

    void ClipItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
    }

    void ClipItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.margin = event.style->getSizeRole(dtk::SizeRole::MarginInside, event.displayScale);
            _size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
            _size.fontInfo = event.style->getFontRole(dtk::FontRole::Label , event.displayScale);
            _size.fontMetrics = event.fontSystem->getMetrics(_size.fontInfo);
            _size.textSize = event.fontSystem->getSize(_text, _size.fontInfo);
            _draw.glyphs.clear();
        }
        dtk::Size2I sizeHint(
            _timeRange.duration().rescaled_to(1.0).value() * _scale,
            _size.textSize.h + _size.margin + _size.border);
        _setSizeHint(sizeHint);
    }

    void ClipItem::clipEvent(const dtk::Box2I& clipRect, bool clipped)
    {
        IItem::clipEvent(clipRect, clipped);
        if (clipped)
        {
            _draw.glyphs.clear();
        }
    }

    void ClipItem::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItem::drawEvent(drawRect, event);
        const dtk::Box2I& g = getGeometry();

        const dtk::Box2I g2 = dtk::margin(g, -_size.border);
        event.render->drawRect(
            dtk::Box2F(g2.min.x, g2.min.y, g2.w(), g2.h()),
            _color);

        const dtk::Box2I g3 = dtk::margin(g2, -_size.margin);
        if (!_text.empty() && _draw.glyphs.empty())
        {
            _draw.glyphs = event.fontSystem->getGlyphs(_text, _size.fontInfo);
        }
        dtk::ClipRectEnabledState clipRectEnabledState(event.render);
        dtk::ClipRectState clipRectState(event.render);
        event.render->setClipRectEnabled(true);
        event.render->setClipRect(g3);
        event.render->drawText(
            _draw.glyphs,
            _size.fontMetrics,
            dtk::V2F(g3.min.x, g3.min.y + g3.h() / 2 - _size.fontMetrics.lineHeight / 2),
            event.style->getColorRole(dtk::ColorRole::Text));
    }

    void TrackItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track,
        const std::shared_ptr<IWidget>& parent)
    {
        auto opt = track->trimmed_range_in_parent();
        IItem::_init(
            context,
            opt.has_value() ? opt.value() : OTIO_NS::TimeRange(),
            "toucan::TrackItem",
            parent);

        _track = track;

        dtk::Random random;
        for (const auto& child : track->children())
        {
            if (auto clip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(child))
            {
                auto clipItem = ClipItem::create(context, clip, shared_from_this());
                clipItem->setColor(dtk::Color4F(
                    random.getF(.5F) + .2F,
                    random.getF(.5F) + .2F,
                    random.getF(.5F) + .2F));
            }
        }
    }

    TrackItem::~TrackItem()
    {}

    std::shared_ptr<TrackItem> TrackItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<TrackItem>();
        out->_init(context, track, parent);
        return out;
    }

    void TrackItem::setColor(const dtk::Color4F& value)
    {
        if (value == _color)
            return;
        _color = value;
        _setDrawUpdate();
    }

    void TrackItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        const dtk::Box2I g = dtk::margin(value, 0, -_size.margin, 0, -_size.margin);
        dtk::V2I pos = g.min;
        for (const auto& child : getChildren())
        {
            const dtk::Size2I& sizeHint = child->getSizeHint();
            child->setGeometry(dtk::Box2I(pos.x, pos.y, sizeHint.w, g.h()));
            pos.x += sizeHint.w;
        }
    }

    void TrackItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.margin = event.style->getSizeRole(dtk::SizeRole::MarginInside, event.displayScale);
        }
        dtk::Size2I sizeHint(
            _timeRange.duration().rescaled_to(1.0).value() * _scale,
            0);
        for (const auto& child : getChildren())
        {
            const dtk::Size2I& childSizeHint = child->getSizeHint();
            sizeHint.h = std::max(sizeHint.h, childSizeHint.h);
        }
        sizeHint.h += _size.margin * 2;
        _setSizeHint(sizeHint);
    }

    void TimelineItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
        const std::shared_ptr<IWidget>& parent)
    {
        const OTIO_NS::RationalTime& duration = timeline->duration();
        OTIO_NS::RationalTime startTime(0.0, duration.rate());
        auto opt = timeline->global_start_time();
        if (opt.has_value())
        {
            startTime = opt.value();
        }
        IItem::_init(
            context,
            OTIO_NS::TimeRange(startTime, duration),
            "toucan::TimelineItem",
            parent);

        _setMouseHoverEnabled(true);
        _setMousePressEnabled(true);

        _timeline = timeline;
        _timeUnitsModel = timeUnitsModel;

        for (const auto& track : timeline->video_tracks())
        {
            auto trackItem = TrackItem::create(context, track, shared_from_this());
        }

        _timeUnitsObserver = dtk::ValueObserver<TimeUnits>::create(
            timeUnitsModel->observeTimeUnits(),
            [this](TimeUnits)
            {
                _setDrawUpdate();
            });
    }

    TimelineItem::~TimelineItem()
    {}

    std::shared_ptr<TimelineItem> TimelineItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<TimelineItem>();
        out->_init(context, timeline, timeUnitsModel, parent);
        return out;
    }

    const OTIO_NS::RationalTime& TimelineItem::getCurrentTime() const
    {
        return _currentTime;
    }

    void TimelineItem::setCurrentTime(const OTIO_NS::RationalTime& value)
    {
        if (value == _currentTime)
            return;
        _currentTime = value;
        _setDrawUpdate();
    }

    void TimelineItem::setCurrentTimeCallback(const std::function<void(const OTIO_NS::RationalTime&)>& value)
    {
        _currentTimeCallback = value;
    }

    void TimelineItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        dtk::V2I pos = value.min;
        pos.y += _size.fontMetrics.lineHeight + _size.margin * 2;
        for (const auto& child : getChildren())
        {
            const dtk::Size2I& sizeHint = child->getSizeHint();
            child->setGeometry(dtk::Box2I(pos.x, pos.y, sizeHint.w, sizeHint.h));
            pos.y += sizeHint.h + _size.spacing;
        }
        if (auto scrollArea = getParentT<dtk::ScrollArea>())
        {
            _size.scrollPos = scrollArea->getScrollPos();
        }
    }

    void TimelineItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.margin = event.style->getSizeRole(dtk::SizeRole::MarginInside, event.displayScale);
            _size.spacing = event.style->getSizeRole(dtk::SizeRole::SpacingTool, event.displayScale);
            _size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
            _size.fontInfo = event.style->getFontRole(dtk::FontRole::Label, event.displayScale);
            _size.fontMetrics = event.fontSystem->getMetrics(_size.fontInfo);
        }
        dtk::Size2I sizeHint(
            _timeRange.duration().rescaled_to(1.0).value() * _scale,
            _size.fontMetrics.lineHeight + _size.margin * 2);
        const auto& children = getChildren();
        for (const auto& child : children)
        {
            const dtk::Size2I& childSizeHint = child->getSizeHint();
            sizeHint.h += childSizeHint.h;
        }
        if (!children.empty())
        {
            sizeHint.h += _size.spacing * (children.size() - 1);
        }
        _setSizeHint(sizeHint);
    }

    void TimelineItem::drawOverlayEvent(const dtk::Box2I& drawRect, const dtk::DrawEvent& event)
    {
        IItem::drawOverlayEvent(drawRect, event);
        const dtk::Box2I& g = getGeometry();

        const dtk::Box2I g2(
            g.min.x,
            g.min.y + _size.scrollPos.y,
            g.w(),
            _size.fontMetrics.lineHeight + _size.margin * 2);
        event.render->drawRect(
            dtk::Box2F(g2.min.x, g2.min.y, g2.w(), g2.h()),
            event.style->getColorRole(dtk::ColorRole::Base));

        int pos = _timeToPos(_currentTime);
        event.render->drawRect(
            dtk::Box2F(pos, g.min.y + _size.scrollPos.y, _size.border * 2, g.h()),
            event.style->getColorRole(dtk::ColorRole::Red));

        std::string s = _timeUnitsModel->getLabel(_currentTime);
        dtk::Size2I size = event.fontSystem->getSize(s, _size.fontInfo);
        dtk::Box2I g3(
            pos + _size.border * 2 + _size.margin,
            g.min.y + _size.scrollPos.y + _size.margin,
            size.w,
            _size.fontMetrics.lineHeight);
        if (g3.max.x > g.max.x)
        {
            g3.min.x -= size.w + _size.border * 2 + _size.margin;
            g3.max.x -= size.w + _size.border * 2 + _size.margin;
        }
        event.render->drawText(
            event.fontSystem->getGlyphs(s, _size.fontInfo),
            _size.fontMetrics,
            dtk::V2F(g3.min.x, g3.min.y),
            event.style->getColorRole(dtk::ColorRole::Text));
    }

    void TimelineItem::mouseMoveEvent(dtk::MouseMoveEvent& event)
    {
        IItem::mouseMoveEvent(event);
        if (_isMousePressed())
        {
            _currentTime = _posToTime(_getMousePos().x);
            if (_currentTimeCallback)
            {
                _currentTimeCallback(_currentTime);
            }
            _setDrawUpdate();
        }
    }

    void TimelineItem::mousePressEvent(dtk::MouseClickEvent& event)
    {
        IItem::mousePressEvent(event);
        _currentTime = _posToTime(_getMousePos().x);
        if (_currentTimeCallback)
        {
            _currentTimeCallback(_currentTime);
        }
        _setDrawUpdate();
    }

    OTIO_NS::RationalTime TimelineItem::_posToTime(double value) const
    {
        OTIO_NS::RationalTime out;
        const dtk::Box2I& g = getGeometry();
        if (g.w() > 0)
        {
            const double normalized = (value - g.min.x) /
                static_cast<double>(_timeRange.duration().rescaled_to(1.0).value() * _scale);
            out = OTIO_NS::RationalTime(
                _timeRange.start_time() +
                OTIO_NS::RationalTime(
                    _timeRange.duration().value() * normalized,
                    _timeRange.duration().rate())).
                round();
            out = dtk::clamp(
                out,
                _timeRange.start_time(),
                _timeRange.end_time_inclusive());
        }
        return out;
    }

    int TimelineItem::_timeToPos(const OTIO_NS::RationalTime& value) const
    {
        const dtk::Box2I& g = getGeometry();
        const OTIO_NS::RationalTime t = value - _timeRange.start_time();
        return g.min.x + t.rescaled_to(1.0).value() * _scale;
    }

    void TimelineWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::TimelineWidget", parent);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, shared_from_this());

        std::weak_ptr<App> appWeak(app);
        _documentObserver = dtk::ValueObserver<std::shared_ptr<Document> >::create(
            app->getDocumentsModel()->observeCurrent(),
            [this, appWeak](const std::shared_ptr<Document>& document)
            {
                auto context = _getContext().lock();
                auto app = appWeak.lock();
                _document = document;
                if (document)
                {
                    auto timeline = _document->getTimeline();
                    _duration = timeline->duration();
                    _timelineItem = TimelineItem::create(
                        context,
                        timeline,
                        app->getTimeUnitsModel());
                    _timelineItem->setCurrentTimeCallback(
                        [this](const OTIO_NS::RationalTime& value)
                        {
                            _document->getPlaybackModel()->setCurrentTime(value);
                        });

                    _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
                        _document->getPlaybackModel()->observeCurrentTime(),
                        [this](const OTIO_NS::RationalTime& value)
                        {
                            if (_timelineItem)
                            {
                                _timelineItem->setCurrentTime(value);
                            }
                        });
                }
                else
                {
                    _duration = OTIO_NS::RationalTime();
                    _timelineItem.reset();
                    _currentTimeObserver.reset();
                }
                _scrollWidget->setWidget(_timelineItem);
            });
    }

    TimelineWidget::~TimelineWidget()
    {}

    std::shared_ptr<TimelineWidget> TimelineWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<TimelineWidget>(new TimelineWidget);
        out->_init(context, app, parent);
        return out;
    }

    void TimelineWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _scrollWidget->setGeometry(value);
        if (_timelineItem)
        {
            _timelineItem->setScale(
                _scrollWidget->getViewport().w() /
                _duration.rescaled_to(1.0).value());
        }
    }

    void TimelineWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_scrollWidget->getSizeHint());
    }
}
