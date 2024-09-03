// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Viewport.h"

#include "App.h"

namespace toucan
{
    void Viewport::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::Viewport", parent);

        _imageObserver = dtk::ValueObserver<std::shared_ptr<dtk::Image> >::create(
            app->observeCurrentImage(),
            [this](const std::shared_ptr<dtk::Image>& value)
            {
                _image = value;
                _setDrawUpdate();
            });
    }

    Viewport::~Viewport()
    {}

    std::shared_ptr<Viewport> Viewport::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<Viewport>(new Viewport);
        out->_init(context, app, parent);
        return out;
    }

    void Viewport::drawEvent(const dtk::Box2I& drawRect, const dtk::DrawEvent& event)
    {
        IWidget::drawEvent(drawRect, event);
        if (_image)
        {
            const dtk::Box2I& g = getGeometry();

            const dtk::Size2I viewportSize(g.w(), g.h());
            const dtk::Size2I imageSize = _image->getSize();
            double zoom = viewportSize.w / static_cast<double>(imageSize.w);
            if (zoom * imageSize.h > viewportSize.h)
            {
                zoom = viewportSize.h / static_cast<double>(imageSize.h);
            }
            const dtk::V2I c(imageSize.w / 2, imageSize.h / 2);
            const dtk::V2I viewPos(
                viewportSize.w / 2.F - c.x * zoom,
                viewportSize.h / 2.F - c.y * zoom);

            dtk::M44F vm;
            vm = vm * dtk::translate(dtk::V3F(g.min.x, g.min.y, 0.F));
            vm = vm * dtk::translate(dtk::V3F(viewPos.x, viewPos.y, 0.F));
            vm = vm * dtk::scale(dtk::V3F(zoom, zoom, 1.F));
            const auto m = event.render->getTransform();
            event.render->setTransform(m * vm);
            event.render->drawImage(
                _image,
                dtk::Box2F(0, 0, imageSize.w, imageSize.h));
            event.render->setTransform(m);
        }
    }
}
