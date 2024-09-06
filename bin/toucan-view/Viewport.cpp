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

        _setMouseHoverEnabled(true);
        _setMousePressEnabled(true);

        _documentObserver = dtk::ValueObserver<std::shared_ptr<Document> >::create(
            app->getDocumentsModel()->observeCurrent(),
            [this](const std::shared_ptr<Document>& document)
            {
                _document = document;
                if (document)
                {
                    document->getViewModel()->setViewportSize(getGeometry().size());
                    document->getViewModel()->setMouseInside(_isMouseInside());
                    document->getViewModel()->setMousePos(_getMousePos());
                    document->getViewModel()->setMousePress(false);

                    _imageObserver = dtk::ValueObserver<std::shared_ptr<dtk::Image> >::create(
                        document->observeCurrentImage(),
                        [this](const std::shared_ptr<dtk::Image>& value)
                        {
                            _image = value;
                            if (_document)
                            {
                                _document->getViewModel()->setImageSize(value ? value->getSize() : dtk::Size2I());
                            }
                            _setDrawUpdate();
                        });

                    _viewPosObserver = dtk::ValueObserver<dtk::V2I>::create(
                        document->getViewModel()->observePos(),
                        [this](const dtk::V2I& value)
                        {
                            _viewPos = value;
                            _setDrawUpdate();
                        });

                    _viewZoomObserver = dtk::ValueObserver<float>::create(
                        document->getViewModel()->observeZoom(),
                        [this](float value)
                        {
                            _viewZoom = value;
                            _setDrawUpdate();
                        });

                    _frameViewObserver = dtk::ValueObserver<bool>::create(
                        document->getViewModel()->observeFrame(),
                        [this](bool value)
                        {
                            _frameView = value;
                            _setDrawUpdate();
                        });
                }
                else
                {
                    _image.reset();
                    _viewPos = dtk::V2I();
                    _viewZoom = 1.F;
                    _frameView = true;

                    _setDrawUpdate();

                    _imageObserver.reset();
                    _viewPosObserver.reset();
                    _viewZoomObserver.reset();
                    _frameViewObserver.reset();
                }
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
        if (_document)
        {
            _document->getViewModel()->setViewportSize(value.size());
        }
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

    void Viewport::mouseEnterEvent(dtk::MouseEnterEvent& event)
    {
        IWidget::mouseEnterEvent(event);
        if (_document)
        {
            _document->getViewModel()->setMouseInside(true);
        }
    }

    void Viewport::mouseLeaveEvent()
    {
        IWidget::mouseLeaveEvent();
        if (_document)
        {
            _document->getViewModel()->setMouseInside(false);
        }
    }

    void Viewport::mouseMoveEvent(dtk::MouseMoveEvent& event)
    {
        IWidget::mouseMoveEvent(event);
        if (_document)
        {
            event.accept = true;
            const dtk::Box2I& g = getGeometry();
            _document->getViewModel()->setMousePos(event.pos - g.min);
        }
    }

    void Viewport::mousePressEvent(dtk::MouseClickEvent& event)
    {
        IWidget::mousePressEvent(event);
        if (_document && 0 == event.button && 0 == event.modifiers)
        {
            event.accept = true;
            _document->getViewModel()->setMousePress(true);
        }
    }

    void Viewport::mouseReleaseEvent(dtk::MouseClickEvent& event)
    {
        IWidget::mouseReleaseEvent(event);
        if (_document && 0 == event.button && 0 == event.modifiers)
        {
            event.accept = true;
            _document->getViewModel()->setMousePress(false);
        }
    }

    void Viewport::scrollEvent(dtk::ScrollEvent& event)
    {
        IWidget::scrollEvent(event);
        if (_document && 0 == event.modifiers)
        {
            event.accept = true;
            if (event.value.y > 0)
            {
                _document->getViewModel()->zoomOut(.9F);
            }
            else if (event.value.y < 0)
            {
                _document->getViewModel()->zoomIn(.9F);
            }
        }
    }
}
