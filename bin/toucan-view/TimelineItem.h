// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "IItem.h"
#include "SelectionModel.h"
#include "TimeUnitsModel.h"

namespace toucan
{
    class Document;

    class TimelineItem : public IItem
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimelineItem();

        static std::shared_ptr<TimelineItem> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        const OTIO_NS::RationalTime& getCurrentTime() const;
        void setCurrentTime(const OTIO_NS::RationalTime&);
        void setCurrentTimeCallback(const std::function<void(const OTIO_NS::RationalTime&)>&);

        OTIO_NS::RationalTime posToTime(double) const;
        int timeToPos(const OTIO_NS::RationalTime&) const;

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void drawOverlayEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;
        void mouseMoveEvent(dtk::MouseMoveEvent&) override;
        void mousePressEvent(dtk::MouseClickEvent&) override;
        void mouseReleaseEvent(dtk::MouseClickEvent&) override;

    private:
        dtk::Size2I _getLabelMaxSize(
            const std::shared_ptr<dtk::FontSystem>&) const;
        void _getTimeTicks(
            const std::shared_ptr<dtk::FontSystem>&,
            double& seconds,
            int& tick);
        void _drawTimeTicks(
            const dtk::Box2I&,
            const dtk::DrawEvent&);
        void _drawTimeLabels(
            const dtk::Box2I&,
            const dtk::DrawEvent&);

        bool _select(
            const std::shared_ptr<dtk::IWidget>&,
            const dtk::V2I&,
            std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >&);
        void _select(
            const std::shared_ptr<dtk::IWidget>&,
            const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >&);

        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> _timeline;
        OTIO_NS::RationalTime _currentTime = OTIO_NS::RationalTime(-1.0, -1.0);
        std::function<void(const OTIO_NS::RationalTime&)> _currentTimeCallback;
        std::shared_ptr<TimeUnitsModel> _timeUnitsModel;
        std::shared_ptr<SelectionModel> _selectionModel;

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int margin = 0;
            int spacing = 0;
            int border = 0;
            int handle = 0;
            dtk::FontInfo fontInfo;
            dtk::FontMetrics fontMetrics;
            dtk::V2I scrollPos;
        };
        SizeData _size;

        enum MouseMode
        {
            None,
            CurrentTime,
            Select
        };
        struct MouseData
        {
            MouseMode mode = MouseMode::None;
        };
        MouseData _mouse;

        std::shared_ptr<dtk::ValueObserver<TimeUnits> > _timeUnitsObserver;
        std::shared_ptr<dtk::ListObserver<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > > _selectionObserver;
    };
}
