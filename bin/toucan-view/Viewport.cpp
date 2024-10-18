// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Viewport.h"

#include "App.h"
#include "Document.h"
#include "ViewModel.h"

namespace toucan
{
    void Viewport::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<Document>& document,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::Viewport", parent);

        _setMouseHoverEnabled(true);
        _setMousePressEnabled(true);

        _viewModel = document->getViewModel();
        _pos = dtk::ObservableValue<dtk::V2I>::create();
        _zoom = dtk::ObservableValue<float>::create(1.F);
        _frame = dtk::ObservableValue<bool>::create(true);

        _imageObserver = dtk::ValueObserver<std::shared_ptr<dtk::Image> >::create(
            document->observeCurrentImage(),
            [this](const std::shared_ptr<dtk::Image>& value)
            {
                _image = value;
                _setDrawUpdate();
            });

        _zoomInObserver = dtk::ValueObserver<bool>::create(
            _viewModel->observeZoomIn(),
            [this](bool value)
            {
                if (value)
                {
                    zoomIn();
                }
            });

        _zoomOutObserver = dtk::ValueObserver<bool>::create(
            _viewModel->observeZoomOut(),
            [this](bool value)
            {
                if (value)
                {
                    zoomOut();
                }
            });

        _zoomResetObserver = dtk::ValueObserver<bool>::create(
            _viewModel->observeZoomReset(),
            [this](bool value)
            {
                if (value)
                {
                    zoomReset();
                }
            });

        _frameObserver = dtk::ValueObserver<bool>::create(
            _viewModel->observeFrame(),
            [this](bool value)
            {
                setFrame(value);
            });
    }

    Viewport::~Viewport()
    {}

    std::shared_ptr<Viewport> Viewport::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<Document>& document,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<Viewport>(new Viewport);
        out->_init(context, document, parent);
        return out;
    }

    const dtk::V2I& Viewport::getPos() const
    {
        return _pos->get();
    }

    float Viewport::getZoom() const
    {
        return _zoom->get();
    }

    std::shared_ptr<dtk::IObservableValue<dtk::V2I> > Viewport::observePos() const
    {
        return _pos;
    }

    std::shared_ptr<dtk::IObservableValue<float> > Viewport::observeZoom() const
    {
        return _zoom;
    }

    void Viewport::setPosZoom(const dtk::V2I& pos, float zoom)
    {
        setFrame(false);
        bool changed = _pos->setIfChanged(pos);
        changed |= _zoom->setIfChanged(zoom);
        if (changed)
        {
            _setDrawUpdate();
        }
    }

    void Viewport::setZoom(float value)
    {
        const dtk::Box2I& g = getGeometry();
        const dtk::V2I viewportCenter(g.w() / 2, g.h() / 2);
        setZoom(value, _isMouseInside() ? (_getMousePos() - g.min) : viewportCenter);
    }

    void Viewport::setZoom(float zoom, const dtk::V2I& focus)
    {
        dtk::V2I pos = _pos->get();
        const float zoomPrev = _zoom->get();
        pos.x = focus.x + (pos.x - focus.x) * (zoom / zoomPrev);
        pos.y = focus.y + (pos.y - focus.y) * (zoom / zoomPrev);
        setPosZoom(pos, zoom);
    }

    void Viewport::zoomIn(double amount)
    {
        setZoom(_zoom->get() * amount);
    }

    void Viewport::zoomOut(double amount)
    {
        setZoom(_zoom->get() / amount);
    }

    void Viewport::zoomReset()
    {
        setZoom(1.F);
    }

    bool Viewport::getFrame() const
    {
        return _frame->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > Viewport::observeFrame() const
    {
        return _frame;
    }

    void Viewport::setFrame(bool value)
    {
        if (_frame->setIfChanged(value))
        {
            _viewModel->setFrame(value);
            _setDrawUpdate();
        }
    }

    void Viewport::drawEvent(const dtk::Box2I& drawRect, const dtk::DrawEvent& event)
    {
        IWidget::drawEvent(drawRect, event);
        const dtk::Box2I& g = getGeometry();
        event.render->drawRect(
            g,
            dtk::Color4F(0.F, 0.F, 0.F));
        if (_image)
        {
            if (_frame->get())
            {
                _frameUpdate();
            }
            const dtk::Size2I& imageSize = _image->getSize();
            dtk::M44F vm;
            vm = vm * dtk::translate(dtk::V3F(g.min.x, g.min.y, 0.F));
            vm = vm * dtk::translate(dtk::V3F(_pos->get().x, _pos->get().y, 0.F));
            vm = vm * dtk::scale(dtk::V3F(_zoom->get(), _zoom->get(), 1.F));
            const auto m = event.render->getTransform();
            event.render->setTransform(m * vm);
            dtk::ImageOptions options;
            options.imageFilters.magnify = dtk::ImageFilter::Nearest;
            event.render->drawImage(
                _image,
                dtk::Box2I(0, 0, imageSize.w, imageSize.h),
                dtk::Color4F(1.F, 1.F, 1.F),
                options);
            event.render->setTransform(m);
        }
    }

    void Viewport::mouseMoveEvent(dtk::MouseMoveEvent& event)
    {
        IWidget::mouseMoveEvent(event);
        if (_isMousePressed())
        {
            event.accept = true;
            const dtk::V2I& mousePressPos = _getMousePressPos();
            _pos->setIfChanged(dtk::V2I(
                _viewMousePress.x + (event.pos.x - mousePressPos.x),
                _viewMousePress.y + (event.pos.y - mousePressPos.y)));
            _setDrawUpdate();
        }
    }

    void Viewport::mousePressEvent(dtk::MouseClickEvent& event)
    {
        IWidget::mousePressEvent(event);
        if (0 == event.button && 0 == event.modifiers)
        {
            event.accept = true;
            setFrame(false);
            _viewMousePress = _pos->get();
        }
    }

    void Viewport::mouseReleaseEvent(dtk::MouseClickEvent& event)
    {
        IWidget::mouseReleaseEvent(event);
        event.accept = true;
    }

    void Viewport::scrollEvent(dtk::ScrollEvent& event)
    {
        IWidget::scrollEvent(event);
        if (0 == event.modifiers)
        {
            event.accept = true;
            if (event.value.y > 0)
            {
                zoomOut(.9F);
            }
            else if (event.value.y < 0)
            {
                zoomIn(.9F);
            }
        }
    }

    void Viewport::_frameUpdate()
    {
        if (_frame && _image)
        {
            const dtk::Box2I& g = getGeometry();
            const dtk::Size2I imageSize = _image->getSize();
            float zoom = g.w() / static_cast<float>(imageSize.w);
            if (zoom * imageSize.h > g.h())
            {
                zoom = g.h() / static_cast<double>(imageSize.h);
            }
            const dtk::V2I c(imageSize.w / 2, imageSize.h / 2);
            const dtk::V2I pos = dtk::V2I(
                g.w() / 2.F - c.x * zoom,
                g.h() / 2.F - c.y * zoom);

            _pos->setIfChanged(pos);
            _zoom->setIfChanged(zoom);
        }
    }
}
