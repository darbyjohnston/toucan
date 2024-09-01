// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtkUI/IWidget.h>

#include <dtkCore/Image.h>

namespace toucan
{
    class App;

    class Viewport : public dtk::ui::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~Viewport();

        static std::shared_ptr<Viewport> create(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void drawEvent(const dtk::core::Box2I&, const dtk::ui::DrawEvent&) override;

    private:
        std::shared_ptr<dtk::core::ValueObserver<std::shared_ptr<dtk::core::Image> > > _imageObserver;
        std::shared_ptr<dtk::core::Image> _image;
    };
}

