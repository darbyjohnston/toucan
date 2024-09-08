// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include "DocumentsModel.h"
#include "PlaybackModel.h"

#include "App.h"

#include <dtk/core/Format.h>
#include <dtk/core/Random.h>
#include <dtk/core/RenderUtil.h>

namespace toucan
{
    namespace
    {
        const float marginPercentage = .1F;
    }

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

        setTooltip(dtk::Format(
            "Clip: {0}\n"
            "Range: {1}-{2}@{3}").
            arg(clip->name()).
            arg(_timeRange.start_time().value()).
            arg(_timeRange.end_time_inclusive().value()).
            arg(_timeRange.duration().rate()));
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
        _setMousePressEnabled(true, 0, 0);

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

    OTIO_NS::RationalTime TimelineItem::posToTime(double value) const
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

    int TimelineItem::timeToPos(const OTIO_NS::RationalTime& value) const
    {
        const dtk::Box2I& g = getGeometry();
        const OTIO_NS::RationalTime t = value - _timeRange.start_time();
        return g.min.x + t.rescaled_to(1.0).value() * _scale;
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
            _size.handle = event.style->getSizeRole(dtk::SizeRole::Handle, event.displayScale);
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

        _drawTimeTicks(drawRect, event);
        _drawTimeLabels(drawRect, event);

        int pos = timeToPos(_currentTime);
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
            _currentTime = posToTime(_getMousePos().x);
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
        if (0 == event.button && 0 == event.modifiers)
        {
            _currentTime = posToTime(_getMousePos().x);
            if (_currentTimeCallback)
            {
                _currentTimeCallback(_currentTime);
            }
            _setDrawUpdate();
        }
    }

    dtk::Size2I TimelineItem::_getLabelMaxSize(
        const std::shared_ptr<dtk::FontSystem>& fontSystem) const
    {
        const std::string labelMax = _timeUnitsModel->getLabel(_timeRange.duration());
        const dtk::Size2I labelMaxSize = fontSystem->getSize(labelMax, _size.fontInfo);
        return labelMaxSize;
    }

    void TimelineItem::_getTimeTicks(
        const std::shared_ptr<dtk::FontSystem>& fontSystem,
        double& seconds,
        int& tick)
    {
        const int w = getSizeHint().w;
        const float duration = _timeRange.duration().rescaled_to(1.0).value();
        const int secondsTick = 1.0 / duration * w;
        const int minutesTick = 60.0 / duration * w;
        const int hoursTick = 3600.0 / duration * w;
        const dtk::Size2I labelMaxSize = _getLabelMaxSize(fontSystem);
        const int distanceMin = _size.border + _size.margin + labelMaxSize.w;
        seconds = 0.0;
        tick = 0;
        if (secondsTick >= distanceMin)
        {
            seconds = 1.0;
            tick = secondsTick;
        }
        else if (minutesTick >= distanceMin)
        {
            seconds = 60.0;
            tick = minutesTick;
        }
        else if (hoursTick >= distanceMin)
        {
            seconds = 3600.0;
            tick = hoursTick;
        }
    }

    void TimelineItem::_drawTimeTicks(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        if (_timeRange != OTIO_NS::TimeRange())
        {
            const dtk::Box2I& g = getGeometry();
            const int w = getSizeHint().w;
            const float duration = _timeRange.duration().rescaled_to(1.0).value();
            const int frameTick = 1.0 / _timeRange.duration().value() * w;
            if (duration > 0.0 && frameTick >= _size.handle)
            {
                dtk::TriMesh2F mesh;
                size_t i = 1;
                const OTIO_NS::RationalTime t0 = posToTime(g.min.x) - _timeRange.start_time();
                const OTIO_NS::RationalTime t1 = posToTime(g.max.x) - _timeRange.start_time();
                const double inc = 1.0 / _timeRange.duration().rate();
                const double x0 = static_cast<int>(t0.rescaled_to(1.0).value() / inc) * inc;
                const double x1 = static_cast<int>(t1.rescaled_to(1.0).value() / inc) * inc;
                for (double t = x0; t <= x1; t += inc)
                {
                    const dtk::Box2I box(
                        g.min.x +
                        t / duration * w,
                        _size.scrollPos.y +
                        g.min.y +
                        _size.fontMetrics.lineHeight,
                        _size.border,
                        _size.margin * 2);
                    if (intersects(box, drawRect))
                    {
                        mesh.v.push_back(dtk::V2F(box.min.x, box.min.y));
                        mesh.v.push_back(dtk::V2F(box.max.x + 1, box.min.y));
                        mesh.v.push_back(dtk::V2F(box.max.x + 1, box.max.y + 1));
                        mesh.v.push_back(dtk::V2F(box.min.x, box.max.y + 1));
                        mesh.triangles.push_back({ i + 0, i + 1, i + 2 });
                        mesh.triangles.push_back({ i + 2, i + 3, i + 0 });
                        i += 4;
                    }
                }
                if (!mesh.v.empty())
                {
                    event.render->drawMesh(
                        mesh,
                        event.style->getColorRole(dtk::ColorRole::Button));
                }
            }

            double seconds = 0;
            int tick = 0;
            _getTimeTicks(event.fontSystem, seconds, tick);
            if (duration > 0.0 && seconds > 0.0 && tick > 0)
            {
                dtk::TriMesh2F mesh;
                size_t i = 1;
                const OTIO_NS::RationalTime t0 = posToTime(g.min.x) - _timeRange.start_time();
                const OTIO_NS::RationalTime t1 = posToTime(g.max.x) - _timeRange.start_time();
                const double inc = seconds;
                const double x0 = static_cast<int>(t0.rescaled_to(1.0).value() / inc) * inc;
                const double x1 = static_cast<int>(t1.rescaled_to(1.0).value() / inc) * inc;
                for (double t = x0; t <= x1; t += inc)
                {
                    const dtk::Box2I box(
                        g.min.x +
                        t / duration * w,
                        _size.scrollPos.y +
                        g.min.y,
                        _size.border,
                        _size.margin +
                        _size.fontMetrics.lineHeight +
                        _size.margin * 2);
                    if (intersects(box, drawRect))
                    {
                        mesh.v.push_back(dtk::V2F(box.min.x, box.min.y));
                        mesh.v.push_back(dtk::V2F(box.max.x + 1, box.min.y));
                        mesh.v.push_back(dtk::V2F(box.max.x + 1, box.max.y + 1));
                        mesh.v.push_back(dtk::V2F(box.min.x, box.max.y + 1));
                        mesh.triangles.push_back({ i + 0, i + 1, i + 2 });
                        mesh.triangles.push_back({ i + 2, i + 3, i + 0 });
                        i += 4;
                    }
                }
                if (!mesh.v.empty())
                {
                    event.render->drawMesh(
                        mesh,
                        event.style->getColorRole(dtk::ColorRole::Button));
                }
            }
        }
    }

    void TimelineItem::_drawTimeLabels(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        if (_timeRange != OTIO_NS::TimeRange())
        {
            const dtk::Box2I& g = getGeometry();
            const int w = getSizeHint().w;
            const float duration = _timeRange.duration().rescaled_to(1.0).value();
            double seconds = 0;
            int tick = 0;
            _getTimeTicks(event.fontSystem, seconds, tick);
            if (seconds > 0.0 && tick > 0)
            {
                const dtk::Size2I labelMaxSize = _getLabelMaxSize(event.fontSystem);
                const OTIO_NS::RationalTime t0 = posToTime(g.min.x) - _timeRange.start_time();
                const OTIO_NS::RationalTime t1 = posToTime(g.max.x) - _timeRange.start_time();
                const double inc = seconds;
                const double x0 = static_cast<int>(t0.rescaled_to(1.0).value() / inc) * inc;
                const double x1 = static_cast<int>(t1.rescaled_to(1.0).value() / inc) * inc;
                for (double t = x0; t <= x1; t += inc)
                {
                    const OTIO_NS::RationalTime time = _timeRange.start_time() +
                        OTIO_NS::RationalTime(t, 1.0).rescaled_to(_timeRange.duration().rate());
                    const dtk::Box2I box(
                        g.min.x +
                        t / duration * w +
                        _size.border +
                        _size.margin,
                        _size.scrollPos.y +
                        g.min.y +
                        _size.margin,
                        labelMaxSize.w,
                        _size.fontMetrics.lineHeight);
                    if (time != _currentTime && intersects(box, drawRect))
                    {
                        const std::string label = _timeUnitsModel->getLabel(time);
                        event.render->drawText(
                            event.fontSystem->getGlyphs(label, _size.fontInfo),
                            _size.fontMetrics,
                            dtk::V2F(box.min.x, box.min.y),
                            event.style->getColorRole(dtk::ColorRole::TextDisabled));
                    }
                }
            }
        }
    }

    void TimelineWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::TimelineWidget", parent);

        _setMouseHoverEnabled(true);
        _setMousePressEnabled(true, 0, static_cast<int>(dtk::KeyModifier::Control));

        _frameView = dtk::ObservableValue<bool>::create(true);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, shared_from_this());
        _scrollWidget->setScrollEventsEnabled(false);

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
                    _timelineItem = TimelineItem::create(
                        context,
                        timeline,
                        app->getTimeUnitsModel());
                    _timelineItem->setCurrentTimeCallback(
                        [this](const OTIO_NS::RationalTime& value)
                        {
                            _document->getPlaybackModel()->setCurrentTime(value);
                        });

                    _timeRangeObserver = dtk::ValueObserver<OTIO_NS::TimeRange>::create(
                        _document->getPlaybackModel()->observeTimeRange(),
                        [this](const OTIO_NS::TimeRange& value)
                        {
                            _timeRange = value;
                        });

                    _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
                        _document->getPlaybackModel()->observeCurrentTime(),
                        [this](const OTIO_NS::RationalTime& value)
                        {
                            _currentTime = value;
                            if (_timelineItem)
                            {
                                _timelineItem->setCurrentTime(value);
                            }
                            _scrollUpdate();
                        });
                }
                else
                {
                    _timeRange = OTIO_NS::TimeRange();
                    _currentTime = OTIO_NS::RationalTime();
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

    void TimelineWidget::setViewZoom(double value)
    {
        const dtk::Box2I& g = getGeometry();
        setViewZoom(value, dtk::V2I(g.w() / 2, g.h() / 2));
    }

    void TimelineWidget::setViewZoom(double zoom, const dtk::V2I& focus)
    {
        _setViewZoom(
            zoom,
            _scale,
            focus,
            _scrollWidget->getScrollPos());
    }

    void TimelineWidget::frameView()
    {
        _scrollWidget->setScrollPos(dtk::V2I());
        const double scale = _getTimelineScale();
        if (scale != _scale)
        {
            _scale = scale;
            if (_timelineItem)
            {
                _timelineItem->setScale(_scale);
            }
            _setSizeUpdate();
            _setDrawUpdate();
        }
    }

    bool TimelineWidget::hasFrameView() const
    {
        return _frameView->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > TimelineWidget::observeFrameView() const
    {
        return _frameView;
    }

    void TimelineWidget::setFrameView(bool value)
    {
        if (_frameView->setIfChanged(value))
        {
            if (value)
            {
                frameView();
            }
        }
    }

    void TimelineWidget::setGeometry(const dtk::Box2I& value)
    {
        const bool changed = value != getGeometry();
        IWidget::setGeometry(value);
        _scrollWidget->setGeometry(value);
        if (_sizeInit || (changed && _frameView->get()))
        {
            _sizeInit = false;
            frameView();
        }
        else if (_timelineItem &&
            _timelineItem->getSizeHint().w <
            _scrollWidget->getViewport().w())
        {
            setFrameView(true);
        }
    }

    void TimelineWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_scrollWidget->getSizeHint());
    }

    void TimelineWidget::mouseMoveEvent(dtk::MouseMoveEvent& event)
    {
        IWidget::mouseMoveEvent(event);
        switch (_mouse.mode)
        {
        case MouseMode::Scroll:
        {
            const dtk::V2I d = event.pos - _getMousePressPos();
            _scrollWidget->setScrollPos(_mouse.scrollPos - d);
            setFrameView(false);
            break;
        }
        default: break;
        }
    }

    void TimelineWidget::mousePressEvent(dtk::MouseClickEvent& event)
    {
        IWidget::mousePressEvent(event);
        if (0 == event.button &&
            static_cast<int>(dtk::KeyModifier::Control) == event.modifiers)
        {
            takeKeyFocus();
            _mouse.mode = MouseMode::Scroll;
            _mouse.scrollPos = _scrollWidget->getScrollPos();
        }
        else
        {
            _mouse.mode = MouseMode::None;
            _mouse.mode = MouseMode::None;
        }
    }

    void TimelineWidget::mouseReleaseEvent(dtk::MouseClickEvent& event)
    {
        IWidget::mouseReleaseEvent(event);
    }

    void TimelineWidget::scrollEvent(dtk::ScrollEvent& event)
    {
        IWidget::scrollEvent(event);
        event.accept = true;
        if (event.value.y > 0)
        {
            const double zoom = _scale * 1.1;
            setViewZoom(zoom, event.pos);
        }
        else
        {
            const double zoom = _scale / 1.1;
            setViewZoom(zoom, event.pos);
        }
    }

    void TimelineWidget::keyPressEvent(dtk::KeyEvent& event)
    {
        if (0 == event.modifiers)
        {
            switch (event.key)
            {
            case dtk::Key::Equal:
                event.accept = true;
                setViewZoom(_scale * 2.0, event.pos);
                break;
            case dtk::Key::Minus:
                event.accept = true;
                setViewZoom(_scale / 2.0, event.pos);
                break;
            case dtk::Key::Backspace:
                event.accept = true;
                setFrameView(true);
                break;
            default: break;
            }
        }
    }

    void TimelineWidget::keyReleaseEvent(dtk::KeyEvent& event)
    {
        event.accept = true;
    }

    void TimelineWidget::_setViewZoom(
        double zoomNew,
        double zoomPrev,
        const dtk::V2I& focus,
        const dtk::V2I& scrollPos)
    {
        const int w = getGeometry().w();
        const double zoomMin = _getTimelineScale();
        const double zoomMax = _getTimelineScaleMax();
        const double zoomClamped = dtk::clamp(zoomNew, zoomMin, zoomMax);
        if (zoomClamped != _scale)
        {
            _scale = zoomClamped;
            if (_timelineItem)
            {
                _timelineItem->setScale(_scale);
            }
            const double s = zoomClamped / zoomPrev;
            const dtk::V2I scrollPosNew(
                (scrollPos.x + focus.x) * s - focus.x,
                scrollPos.y);
            _scrollWidget->setScrollPos(scrollPosNew, false);

            setFrameView(zoomNew <= zoomMin);
        }
    }

    double TimelineWidget::_getTimelineScale() const
    {
        double out = 1.0;
        const double duration = _timeRange.duration().rescaled_to(1.0).value();
        if (duration > 0.0)
        {
            const dtk::Box2I scrollViewport = _scrollWidget->getViewport();
            out = scrollViewport.w() / duration;
        }
        return out;
    }

    double TimelineWidget::_getTimelineScaleMax() const
    {
        double out = 1.0;
        const dtk::Box2I scrollViewport = _scrollWidget->getViewport();
        const double duration = _timeRange.duration().rescaled_to(1.0).value();
        if (duration < 1.0)
        {
            if (duration > 0.0)
            {
                out = scrollViewport.w() / duration;
            }
        }
        else
        {
            out = scrollViewport.w();
        }
        return out;
    }

    void TimelineWidget::_scrollUpdate()
    {
        if (_timelineItem &&
            MouseMode::None == _mouse.mode)
        {
            const int pos = _timelineItem->timeToPos(_currentTime);
            const dtk::Box2I vp = _scrollWidget->getViewport();
            const int margin = vp.w() * marginPercentage;
            if (pos < (vp.min.x + margin) || pos >(vp.max.x - margin))
            {
                const int offset = pos < (vp.min.x + margin) ? (vp.min.x + margin) : (vp.max.x - margin);
                const OTIO_NS::RationalTime t = _currentTime - _timeRange.start_time();
                dtk::V2I scrollPos = _scrollWidget->getScrollPos();
                const dtk::Box2I& g = getGeometry();
                scrollPos.x = g.min.x - offset + t.rescaled_to(1.0).value() * _scale;
                _scrollWidget->setScrollPos(scrollPos);
            }
        }
    }
}
