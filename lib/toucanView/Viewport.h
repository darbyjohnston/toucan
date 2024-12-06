// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/IWidget.h>

#include <dtk/core/Image.h>

namespace toucan
{
    class File;
    class ViewModel;

    //! Viewport widget.
    class Viewport : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<File>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~Viewport();

        //! Create a new widget.
        static std::shared_ptr<Viewport> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<File>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        //! Get the view position.
        const dtk::V2I& getViewPos() const;

        //! Get the view zoom.
        float getViewZoom() const;

        //! Observe the view position.
        std::shared_ptr<dtk::IObservableValue<dtk::V2I> > observeViewPos() const;

        //! Observe the view zoom.
        std::shared_ptr<dtk::IObservableValue<float> > observeViewZoom() const;

        //! Set the view position and zoom.
        void setViewPosZoom(const dtk::V2I&, float);

        //! Set the view zoom.
        void setViewZoom(float);

        //! Set the view zoom.
        void setViewZoom(float, const dtk::V2I& focus);

        //! Zoom in the view.
        void viewZoomIn(double amount = 2.F);

        //! Zoom out the view.
        void viewZoomOut(double amount = 2.F);

        //! Reset the view zoom.
        void viewZoomReset();

        //! Get whether frame view is enabled.
        bool getFrameView() const;

        //! Observe whether frame view is enabled.
        std::shared_ptr<dtk::IObservableValue<bool> > observeFrameView() const;

        //! Set whether frame view is enabled.
        void setFrameView(bool);

        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;
        void mouseMoveEvent(dtk::MouseMoveEvent&) override;
        void mousePressEvent(dtk::MouseClickEvent&) override;
        void mouseReleaseEvent(dtk::MouseClickEvent&) override;
        void scrollEvent(dtk::ScrollEvent&) override;

    private:
        void _frameUpdate();

        std::shared_ptr<ViewModel> _viewModel;
        std::shared_ptr<dtk::Image> _image;
        std::shared_ptr<dtk::ObservableValue<dtk::V2I> > _viewPos;
        std::shared_ptr<dtk::ObservableValue<float> > _viewZoom;
        std::shared_ptr<dtk::ObservableValue<bool> > _frameView;
        dtk::V2I _viewMousePress;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<dtk::Image> > > _imageObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _zoomInObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _zoomOutObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _zoomResetObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _frameObserver;
    };
}

