// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/IWidget.h>

#include <dtk/core/Image.h>

namespace toucan
{
    class App;

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

        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;

    private:
        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<dtk::Image> > > _imageObserver;
        std::shared_ptr<dtk::Image> _image;
    };
}

