// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/IWidget.h>

#include <dtk/core/Image.h>

namespace toucan
{
    class Document;
    class ViewModel;

    class Viewport : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~Viewport();

        static std::shared_ptr<Viewport> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        const dtk::V2I& getPos() const;
        float getZoom() const;
        std::shared_ptr<dtk::IObservableValue<dtk::V2I> > observePos() const;
        std::shared_ptr<dtk::IObservableValue<float> > observeZoom() const;
        void setPosZoom(const dtk::V2I&, float);
        void setZoom(float);
        void setZoom(float, const dtk::V2I& focus);
        void zoomIn(double amount = 2.F);
        void zoomOut(double amount = 2.F);
        void zoomReset();

        bool getFrame() const;
        std::shared_ptr<dtk::IObservableValue<bool> > observeFrame() const;
        void setFrame(bool);

        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;
        void mouseMoveEvent(dtk::MouseMoveEvent&) override;
        void mousePressEvent(dtk::MouseClickEvent&) override;
        void mouseReleaseEvent(dtk::MouseClickEvent&) override;
        void scrollEvent(dtk::ScrollEvent&) override;

    private:
        void _frameUpdate();

        std::shared_ptr<ViewModel> _viewModel;
        std::shared_ptr<dtk::Image> _image;
        std::shared_ptr<dtk::ObservableValue<dtk::V2I> > _pos;
        std::shared_ptr<dtk::ObservableValue<float> > _zoom;
        std::shared_ptr<dtk::ObservableValue<bool> > _frame;
        dtk::V2I _viewMousePress;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<dtk::Image> > > _imageObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _zoomInObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _zoomOutObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _zoomResetObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _frameObserver;
    };
}

