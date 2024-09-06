// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/IWidget.h>

#include <dtk/core/Image.h>

namespace toucan
{
    class App;
    class Document;

    class Viewport : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~Viewport();

        static std::shared_ptr<Viewport> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;
        void mouseEnterEvent(dtk::MouseEnterEvent&) override;
        void mouseLeaveEvent() override;
        void mouseMoveEvent(dtk::MouseMoveEvent&) override;
        void mousePressEvent(dtk::MouseClickEvent&) override;
        void mouseReleaseEvent(dtk::MouseClickEvent&) override;
        void scrollEvent(dtk::ScrollEvent&) override;

    private:
        std::shared_ptr<Document> _document;
        std::shared_ptr<dtk::Image> _image;
        dtk::V2I _viewPos;
        float _viewZoom = 1.F;
        bool _frameView = true;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<Document> > > _documentObserver;
        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<dtk::Image> > > _imageObserver;
        std::shared_ptr<dtk::ValueObserver<dtk::V2I> > _viewPosObserver;
        std::shared_ptr<dtk::ValueObserver<float> > _viewZoomObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _frameViewObserver;
    };
}

