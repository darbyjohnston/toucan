// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Viewport.h"

#include "App.h"

using namespace dtk;
using namespace dtk::core;
using namespace dtk::ui;

namespace toucan
{
    void Viewport::_init(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::Viewport", parent);

        _imageObserver = ValueObserver<std::shared_ptr<Image> >::create(
            app->observeCurrentImage(),
            [this](const std::shared_ptr<Image>& value)
            {
                _image = value;
                _setDrawUpdate();
            });
    }

    Viewport::~Viewport()
    {}

    std::shared_ptr<Viewport> Viewport::create(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<Viewport>(new Viewport);
        out->_init(context, app, parent);
        return out;
    }

    void Viewport::drawEvent(const Box2I& drawRect, const DrawEvent& event)
    {
        IWidget::drawEvent(drawRect, event);
        if (_image)
        {
            const Box2I& g = getGeometry();
            event.render->drawImage(_image, Box2F(g.x(), g.y(), g.w(), g.h()));
        }
    }
}
