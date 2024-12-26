// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/PlaybackModel.h>

#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Label.h>

namespace toucan
{
    class App;
    class File;

    //! HUD widget.
    class HUDWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<File>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~HUDWidget();

        //! Create a new widget.
        static std::shared_ptr<HUDWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<File>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;

    private:
        void _widgetUpdate();
        
        std::shared_ptr<File> _file;
        OTIO_NS::RationalTime _currentTime;
        OTIO_NS::TimeRange _timeRange;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::map<std::string, std::shared_ptr<dtk::Label> > _labels;

        std::shared_ptr<dtk::ValueObserver<OTIO_NS::RationalTime> > _currentTimeObserver;
        std::shared_ptr<dtk::ValueObserver<OTIO_NS::TimeRange> > _timeRangeObserver;
    };
}

