// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Type.h"
#include "Window.h"

#include <toucan/ImageEffectHost.h>
#include <toucan/ImageGraph.h>

#include <dtkUIApp/App.h>

#include <dtkCore/Image.h>
#include <dtkCore/Timer.h>

#include <opentimelineio/timeline.h>

#include <filesystem>

namespace OTIO_NS
{
    //! \todo Add this to OpenTimelineIO?
    inline bool operator == (
        const SerializableObject::Retainer<Timeline>& a,
        const SerializableObject::Retainer<Timeline>& b)
    {
        return a.value == b.value;
    }
}

namespace toucan
{
    class App : public dtk::ui::App
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            std::vector<std::string>&);

    public:
        virtual ~App();

        static std::shared_ptr<App> create(
            const std::shared_ptr<dtk::core::Context>&,
            std::vector<std::string>&);

        void open(const std::filesystem::path&);
        void close();

        std::shared_ptr<dtk::core::IObservableValue<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> > > observeTimeline() const;

        std::shared_ptr<dtk::core::IObservableValue<OTIO_NS::TimeRange> > observeTimeRange() const;

        std::shared_ptr<dtk::core::IObservableValue<OTIO_NS::RationalTime> > observeCurrentTime() const;
        void setCurrentTime(const OTIO_NS::RationalTime&);

        std::shared_ptr<dtk::core::IObservableValue<Playback> > observePlayback() const;
        void setPlayback(Playback);

        std::shared_ptr<dtk::core::IObservableValue<std::shared_ptr<dtk::core::Image> > > observeCurrentImage() const;

    private:
        void _playbackUpdate();
        void _timerUpdate();
        void _render();

        std::shared_ptr<MessageLog> _messageLog;
        std::filesystem::path _path;
        std::shared_ptr<dtk::core::ObservableValue<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> > > _timeline;
        std::shared_ptr<dtk::core::ObservableValue<OTIO_NS::TimeRange> > _timeRange;
        std::shared_ptr<dtk::core::ObservableValue<OTIO_NS::RationalTime> > _currentTime;
        std::shared_ptr<dtk::core::ObservableValue<Playback> > _playback;
        OIIO::ImageBuf _imageBuf;
        std::shared_ptr<dtk::core::ObservableValue<std::shared_ptr<dtk::core::Image> > > _currentImage;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<ImageGraph> _graph;
        std::shared_ptr<Window> _window;
        std::shared_ptr<dtk::core::Timer> _timer;
    };
}

