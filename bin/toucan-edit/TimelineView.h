// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/ScrollWidget.h>
#include <dtk/core/ObservableList.h>

#include <opentimelineio/version.h>

namespace toucan
{
    class App;
    class Document;
    class TimelineWidget;

    class TimelineView : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimelineView();

        static std::shared_ptr<TimelineView> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setViewZoom(double);
        void setViewZoom(double, const dtk::V2I& focus);

        bool hasFrameView() const;
        void frameView();
        std::shared_ptr<dtk::IObservableValue<bool> > observeFrameView() const;
        void setFrameView(bool);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void mouseMoveEvent(dtk::MouseMoveEvent&) override;
        void mousePressEvent(dtk::MouseClickEvent&) override;
        void mouseReleaseEvent(dtk::MouseClickEvent&) override;
        void scrollEvent(dtk::ScrollEvent&) override;
        void keyPressEvent(dtk::KeyEvent&) override;
        void keyReleaseEvent(dtk::KeyEvent&) override;

    private:
        void _setViewZoom(
            double zoomNew,
            double zoomPrev,
            const dtk::V2I& focus,
            const dtk::V2I& scrollPos);

        double _getTimelineScale() const;
        double _getTimelineScaleMax() const;

        void _scrollUpdate();

        std::shared_ptr<Document> _document;
        OTIO_NS::TimeRange _timeRange;
        OTIO_NS::RationalTime _currentTime;
        double _scale = 100.0;
        bool _sizeInit = true;
        std::shared_ptr<dtk::ObservableValue<bool> > _frameView;

        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<TimelineWidget> _timelineWidget;

        enum class MouseMode
        {
            None,
            Scroll
        };
        struct MouseData
        {
            MouseMode mode = MouseMode::None;
            dtk::V2I scrollPos;
            std::chrono::steady_clock::time_point wheelTimer;
        };
        MouseData _mouse;

        std::shared_ptr<dtk::ValueObserver<OTIO_NS::RationalTime> > _currentTimeObserver;
    };
}
