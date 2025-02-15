// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/TimeUnitsModel.h>
#include <toucanView/TimeWidgets.h>

#include <dtk/ui/ComboBox.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ToolButton.h>

namespace toucan
{
    class App;
    class File;

    //! Playback tool bar.
    class PlaybackBar : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~PlaybackBar();

        //! Create a new tool bar.
        static std::shared_ptr<PlaybackBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _timelineUpdate();
        void _timeRangeUpdate();
        void _currentTimeUpdate();
        void _playbackUpdate();

        std::shared_ptr<File> _file;
        OTIO_NS::TimeRange _timeRange;
        OTIO_NS::RationalTime _currentTime;
        Playback _playback = Playback::Stop;

        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::shared_ptr<FrameButtons> _frameButtons;
        std::shared_ptr<PlaybackButtons> _playbackButtons;
        std::shared_ptr<TimeEdit> _timeEdit;
        std::shared_ptr<TimeLabel> _durationLabel;
        std::shared_ptr<dtk::ComboBox> _timeUnitsComboBox;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
        std::shared_ptr<dtk::ValueObserver<OTIO_NS::TimeRange> > _timeRangeObserver;
        std::shared_ptr<dtk::ValueObserver<OTIO_NS::RationalTime> > _currentTimeObserver;
        std::shared_ptr<dtk::ValueObserver<Playback> > _playbackObserver;
        std::shared_ptr<dtk::ValueObserver<TimeUnits> > _timeUnitsObserver;
    };
}

