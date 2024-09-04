// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Viewport.h"

#include "App.h"
#include "ViewModel.h"

namespace toucan
{
    void Viewport::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::Viewport", parent);

        _viewModel = app->getViewModel();

        _setMouseHoverEnabled(true);
        _setMousePressEnabled(true);

        _imageObserver = dtk::ValueObserver<std::shared_ptr<dtk::Image> >::create(
            app->observeCurrentImage(),
            [this](const std::shared_ptr<dtk::Image>& value)
            {
                _image = value;
                _viewModel->setImageSize(value ? value->getSize() : dtk::Size2I());
                _setDrawUpdate();
            });

        _viewPosObserver = dtk::ValueObserver<dtk::V2I>::create(
            app->getViewModel()->observePos(),
            [this](const dtk::V2I& value)
            {
                _viewPos = value;
                _setDrawUpdate();
            });

        _viewZoomObserver = dtk::ValueObserver<float>::create(
            app->getViewModel()->observeZoom(),
            [this](float value)
            {
                _viewZoom = value;
                _setDrawUpdate();
            });

        _frameViewObserver = dtk::ValueObserver<bool>::create(
            app->getViewModel()->observeFrame(),
            [this](bool value)
            {
                _frameView = value;
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

    void Viewport::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _viewModel->setViewportSize(value.size());
    }

    void Viewport::drawEvent(const dtk::Box2I& drawRect, const dtk::DrawEvent& event)
    {
        IWidget::drawEvent(drawRect, event);
        if (_image)
        {
            const dtk::Box2I& g = getGeometry();
            const dtk::Size2I& imageSize = _image->getSize();
            dtk::M44F vm;
            vm = vm * dtk::translate(dtk::V3F(g.min.x, g.min.y, 0.F));
            vm = vm * dtk::translate(dtk::V3F(_viewPos.x, _viewPos.y, 0.F));
            vm = vm * dtk::scale(dtk::V3F(_viewZoom, _viewZoom, 1.F));
            const auto m = event.render->getTransform();
            event.render->setTransform(m * vm);
            dtk::ImageOptions options;
            options.imageFilters.magnify = dtk::ImageFilter::Nearest;
            event.render->drawImage(
                _image,
                dtk::Box2F(0, 0, imageSize.w, imageSize.h),
                dtk::Color4F(1.F, 1.F, 1.F),
                options);
            event.render->setTransform(m);
        }
    }

    void Viewport::mouseEnterEvent()
    {
        IWidget::mouseEnterEvent();
        _viewModel->setMouseInside(true);
    }

    void Viewport::mouseLeaveEvent()
    {
        IWidget::mouseLeaveEvent();
        _viewModel->setMouseInside(false);
    }

    void Viewport::mouseMoveEvent(dtk::MouseMoveEvent& event)
    {
        IWidget::mouseMoveEvent(event);
        event.accept = true;
        const dtk::Box2I& g = getGeometry();
        _viewModel->setMousePos(event.pos - g.min);
    }

    void Viewport::mousePressEvent(dtk::MouseClickEvent& event)
    {
        IWidget::mousePressEvent(event);
        if (0 == event.button && 0 == event.modifiers)
        {
            event.accept = true;
            _viewModel->setMousePress(true);
        }
    }

    void Viewport::mouseReleaseEvent(dtk::MouseClickEvent& event)
    {
        IWidget::mouseReleaseEvent(event);
        if (0 == event.button && 0 == event.modifiers)
        {
            event.accept = true;
            _viewModel->setMousePress(false);
        }
    }

    void Viewport::scrollEvent(dtk::ScrollEvent& event)
    {
        IWidget::scrollEvent(event);
        if (0 == event.modifiers)
        {
            event.accept = true;
            if (event.value.y > 0)
            {
                _viewModel->zoomOut(.9F);
            }
            else if (event.value.y < 0)
            {
                _viewModel->zoomIn(.9F);
            }
        }
    }
}
