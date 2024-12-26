// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/Settings.h>
#include <dtk/core/Context.h>
#include <dtk/core/ObservableValue.h>

#include <opentimelineio/timeline.h>

namespace toucan
{
    //! View model.
    class ViewModel : public std::enable_shared_from_this<ViewModel>
    {
    public:
        ViewModel(const std::shared_ptr<dtk::Context>&);

        virtual ~ViewModel();

        //! Zoom in.
        void zoomIn();

        //! Zoom out.
        void zoomOut();

        //! Reset the zoom.
        void zoomReset();

        //! Observe the zoom in.
        std::shared_ptr<dtk::IObservableValue<bool> > observeZoomIn() const;

        //! Observe the zoom out.
        std::shared_ptr<dtk::IObservableValue<bool> > observeZoomOut() const;

        //! Observe the zoom reset.
        std::shared_ptr<dtk::IObservableValue<bool> > observeZoomReset() const;

        //! Get whether frame view is enabled.
        bool getFrameView() const;

        //! Observe whether frame view is enabled.
        std::shared_ptr<dtk::IObservableValue<bool> > observeFrameView() const;

        //! Set whether frame view is enabled.
        void setFrameView(bool);

    private:
        std::shared_ptr<dtk::ObservableValue<bool> > _zoomIn;
        std::shared_ptr<dtk::ObservableValue<bool> > _zoomOut;
        std::shared_ptr<dtk::ObservableValue<bool> > _zoomReset;
        std::shared_ptr<dtk::ObservableValue<bool> > _frameView;
    };

    //! Global view model.
    class GlobalViewModel : public std::enable_shared_from_this<GlobalViewModel>
    {
    public:
        GlobalViewModel(const std::shared_ptr<dtk::Context>&);

        virtual ~GlobalViewModel();

        //! Get whether the HUD is visible.
        bool getHUD() const;

        //! Observe whether the HUD is visible.
        std::shared_ptr<dtk::IObservableValue<bool> > observeHUD() const;

        //! Set whether the HUD is visible.
        void setHUD(bool);

    private:
        std::shared_ptr<dtk::Settings> _settings;
        std::shared_ptr<dtk::ObservableValue<bool> > _hud;
    };
}
