// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include "App.h"
#include "DocumentsModel.h"
#include "PlaybackModel.h"
#include "TimelineItem.h"

namespace toucan
{
    namespace
    {
        const float marginPercentage = .1F;
    }

    void TimelineWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Document>& document,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::TimelineWidget", parent);

        _setMouseHoverEnabled(true);
        _setMousePressEnabled(true, 0, static_cast<int>(dtk::KeyModifier::Control));

        _document = document;
        _timeRange = document->getPlaybackModel()->getTimeRange();
        _frameView = dtk::ObservableValue<bool>::create(true);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, shared_from_this());
        _scrollWidget->setScrollEventsEnabled(false);

        _timelineItem = TimelineItem::create(
            context,
            app,
            document);
        _scrollWidget->setWidget(_timelineItem);

        _timelineItem->setCurrentTimeCallback(
            [this](const OTIO_NS::RationalTime& value)
            {
                _document->getPlaybackModel()->setCurrentTime(value);
            });

        _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
            document->getPlaybackModel()->observeCurrentTime(),
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

    TimelineWidget::~TimelineWidget()
    {}

    std::shared_ptr<TimelineWidget> TimelineWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Document>& document,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<TimelineWidget>(new TimelineWidget);
        out->_init(context, app, document, parent);
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
            event.accept = true;
            takeKeyFocus();
            _mouse.mode = MouseMode::Scroll;
            _mouse.scrollPos = _scrollWidget->getScrollPos();
        }
        else
        {
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
