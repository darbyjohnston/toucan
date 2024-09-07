// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "TimeUnitsModel.h"

#include <dtk/ui/ScrollWidget.h>

#include <opentimelineio/timeline.h>

namespace toucan
{
    class App;
    class Document;
    class TimeUnitsModel;

    class IItem : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::TimeRange&,
            const std::string&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~IItem() = 0;

        void setScale(double);

    protected:
        OTIO_NS::TimeRange _timeRange;
        double _scale = 100.0;
    };

    class ClipItem : public IItem
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ClipItem();

        static std::shared_ptr<ClipItem> create(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setColor(const dtk::Color4F&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void clipEvent(const dtk::Box2I&, bool) override;
        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;

    private:
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip> _clip;
        std::string _text;
        dtk::Color4F _color = dtk::Color4F(.4F, .3F, .6F);

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int margin = 0;
            int border = 0;
            dtk::FontInfo fontInfo;
            dtk::FontMetrics fontMetrics;
            dtk::Size2I textSize;
        };
        SizeData _size;

        struct DrawData
        {
            std::vector<std::shared_ptr<dtk::Glyph> > glyphs;
        };
        DrawData _draw;
    };

    class TrackItem : public IItem
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TrackItem();

        static std::shared_ptr<TrackItem> create(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setColor(const dtk::Color4F&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track> _track;
        dtk::Color4F _color = dtk::Color4F(.1F, .1F, .1F);

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int margin = 0;
        };
        SizeData _size;
    };

    class TimelineItem : public IItem
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&,
            const std::shared_ptr<TimeUnitsModel>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimelineItem();

        static std::shared_ptr<TimelineItem> create(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&,
            const std::shared_ptr<TimeUnitsModel>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        const OTIO_NS::RationalTime& getCurrentTime() const;
        void setCurrentTime(const OTIO_NS::RationalTime&);
        void setCurrentTimeCallback(const std::function<void(const OTIO_NS::RationalTime&)>&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void drawOverlayEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;
        void mouseMoveEvent(dtk::MouseMoveEvent&) override;
        void mousePressEvent(dtk::MouseClickEvent&) override;

    private:
        OTIO_NS::RationalTime _posToTime(double) const;
        int _timeToPos(const OTIO_NS::RationalTime&) const;

        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> _timeline;
        OTIO_NS::RationalTime _currentTime = OTIO_NS::RationalTime(-1.0, -1.0);
        std::function<void(const OTIO_NS::RationalTime&)> _currentTimeCallback;
        std::shared_ptr<TimeUnitsModel> _timeUnitsModel;

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int margin = 0;
            int spacing = 0;
            int border = 0;
            dtk::FontInfo fontInfo;
            dtk::FontMetrics fontMetrics;
            dtk::V2I scrollPos;
        };
        SizeData _size;

        std::shared_ptr<dtk::ValueObserver<TimeUnits> > _timeUnitsObserver;
    };

    class TimelineWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimelineWidget();

        static std::shared_ptr<TimelineWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        OTIO_NS::RationalTime _duration;
        std::shared_ptr<Document> _document;

        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<TimelineItem> _timelineItem;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<Document> > > _documentObserver;
        std::shared_ptr<dtk::ValueObserver<OTIO_NS::RationalTime> > _currentTimeObserver;
    };
}
