// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineItem.h"

#include "App.h"
#include "DocumentsModel.h"
#include "TrackItem.h"

#include <dtk/ui/ScrollArea.h>

namespace toucan
{
    void TimelineItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Document>& document,
        const std::shared_ptr<IWidget>& parent)
    {
        const auto& timeline = document->getTimeline();
        const OTIO_NS::RationalTime& duration = timeline->duration();
        OTIO_NS::RationalTime startTime(0.0, duration.rate());
        auto opt = timeline->global_start_time();
        if (opt.has_value())
        {
            startTime = opt.value();
        }
        IItem::_init(
            context,
            app,
            nullptr,
            OTIO_NS::TimeRange(startTime, duration),
            "toucan::TimelineItem",
            parent);

        _setMouseHoverEnabled(true);
        _setMousePressEnabled(
            true,
            0,
            0 | static_cast<int>(dtk::KeyModifier::Shift) | static_cast<int>(dtk::KeyModifier::Control));

        _timeline = timeline;
        _timeUnitsModel = app->getTimeUnitsModel();
        _selectionModel = document->getSelectionModel();

        for (const auto& child : timeline->tracks()->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(child))
            {
                auto trackItem = TrackItem::create(
                    context,
                    app,
                    track,
                    shared_from_this());
            }
        }

        _timeUnitsObserver = dtk::ValueObserver<TimeUnits>::create(
            _timeUnitsModel->observeTimeUnits(),
            [this](TimeUnits)
            {
                _setDrawUpdate();
            });

        _selectionObserver = dtk::ListObserver<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >::create(
            _selectionModel->observeSelection(),
            [this](const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& selection)
            {
                _select(shared_from_this(), selection);
            });

    }

    TimelineItem::~TimelineItem()
    {}

    std::shared_ptr<TimelineItem> TimelineItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Document>& document,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<TimelineItem>();
        out->_init(context, app, document, parent);
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
        event.render->drawRect(g2, event.style->getColorRole(dtk::ColorRole::Base));

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
            g3.min,
            event.style->getColorRole(dtk::ColorRole::Text));
    }

    void TimelineItem::mouseMoveEvent(dtk::MouseMoveEvent& event)
    {
        IItem::mouseMoveEvent(event);
        if (MouseMode::CurrentTime == _mouse.mode)
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
        if (0 == event.button &&
            (0 == event.modifiers ||
                static_cast<int>(dtk::KeyModifier::Shift) == event.modifiers ||
                static_cast<int>(dtk::KeyModifier::Control) == event.modifiers))
        {
            event.accept = true;
            std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > selection;
            _select(shared_from_this(), event.pos, selection);
            if (!selection.empty())
            {
                _mouse.mode = MouseMode::Select;
                auto selectionPrev = _selectionModel->getSelection();
                std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > selectionNew;
                if (static_cast<int>(dtk::KeyModifier::Shift) == event.modifiers)
                {
                    selectionNew = selectionPrev;
                    selectionNew.insert(selectionNew.end(), selection.begin(), selection.end());
                }
                else if (static_cast<int>(dtk::KeyModifier::Control) == event.modifiers)
                {
                    selectionNew = selectionPrev;
                    for (const auto& item : selection)
                    {
                        auto i = std::find(selectionNew.begin(), selectionNew.end(), item);
                        if (i != selectionNew.end())
                        {
                            selectionNew.erase(i);
                        }
                    }
                }
                else
                {
                    selectionNew = selection;
                }
                _selectionModel->setSelection(selectionNew);
            }
            else
            {
                _mouse.mode = MouseMode::CurrentTime;
                _currentTime = posToTime(_getMousePos().x);
                if (_currentTimeCallback)
                {
                    _currentTimeCallback(_currentTime);
                }
                _setDrawUpdate();
            }
        }
    }

    void TimelineItem::mouseReleaseEvent(dtk::MouseClickEvent& event)
    {
        IItem::mouseReleaseEvent(event);
        if (_mouse.mode != MouseMode::None)
        {
            event.accept = true;
            _mouse.mode = MouseMode::None;
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
                            box.min,
                            event.style->getColorRole(dtk::ColorRole::TextDisabled));
                    }
                }
            }
        }
    }

    bool TimelineItem::_select(
        const std::shared_ptr<dtk::IWidget>& widget,
        const dtk::V2I& pos,
        std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& selection)
    {
        bool out = false;
        if (dtk::contains(widget->getGeometry(), pos))
        {
            for (const auto& child : widget->getChildren())
            {
                out |= _select(child, pos, selection);
            }
            if (!out)
            {
                if (auto iitem = std::dynamic_pointer_cast<IItem>(widget))
                {
                    selection.push_back(iitem->getItem());
                    out = true;
                }
            }
        }
        return true;
    }

    void TimelineItem::_select(
        const std::shared_ptr<dtk::IWidget>& widget,
        const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& selection)
    {
        if (auto iitem = std::dynamic_pointer_cast<IItem>(widget))
        {
            const auto i = std::find(selection.begin(), selection.end(), iitem->getItem());
            iitem->setSelected(i != selection.end());
        }
        for (const auto& child : widget->getChildren())
        {
            _select(child, selection);
        }
    }
}
