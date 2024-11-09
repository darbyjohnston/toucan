// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/core/ObservableValue.h>

#include <opentimelineio/timeline.h>

namespace toucan
{
    class ViewModel : public std::enable_shared_from_this<ViewModel>
    {
    public:
        ViewModel();

        virtual ~ViewModel();

        void zoomIn();
        void zoomOut();
        void zoomReset();
        std::shared_ptr<dtk::IObservableValue<bool> > observeZoomIn() const;
        std::shared_ptr<dtk::IObservableValue<bool> > observeZoomOut() const;
        std::shared_ptr<dtk::IObservableValue<bool> > observeZoomReset() const;

        bool getFrame() const;
        std::shared_ptr<dtk::IObservableValue<bool> > observeFrame() const;
        void setFrame(bool);

    private:
        std::shared_ptr<dtk::ObservableValue<bool> > _zoomIn;
        std::shared_ptr<dtk::ObservableValue<bool> > _zoomOut;
        std::shared_ptr<dtk::ObservableValue<bool> > _zoomReset;
        std::shared_ptr<dtk::ObservableValue<bool> > _frame;
    };
}
