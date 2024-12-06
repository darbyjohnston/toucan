// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "Viewport.h"

#include "App.h"
#include "File.h"
#include "ViewModel.h"

namespace toucan
{
    void Viewport::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<File>& file,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::Viewport", parent);

        _setMouseHoverEnabled(true);
        _setMousePressEnabled(true);

        _viewModel = file->getViewModel();
        _viewPos = dtk::ObservableValue<dtk::V2I>::create();
        _viewZoom = dtk::ObservableValue<float>::create(1.F);
        _frameView = dtk::ObservableValue<bool>::create(true);

        _imageObserver = dtk::ValueObserver<std::shared_ptr<dtk::Image> >::create(
            file->observeCurrentImage(),
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
                    viewZoomIn();
                }
            });

        _zoomOutObserver = dtk::ValueObserver<bool>::create(
            _viewModel->observeZoomOut(),
            [this](bool value)
            {
                if (value)
                {
                    viewZoomOut();
                }
            });

        _zoomResetObserver = dtk::ValueObserver<bool>::create(
            _viewModel->observeZoomReset(),
            [this](bool value)
            {
                if (value)
                {
                    viewZoomReset();
                }
            });

        _frameObserver = dtk::ValueObserver<bool>::create(
            _viewModel->observeFrameView(),
            [this](bool value)
            {
                setFrameView(value);
            });
    }

    Viewport::~Viewport()
    {}

    std::shared_ptr<Viewport> Viewport::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<File>& file,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<Viewport>(new Viewport);
        out->_init(context, file, parent);
        return out;
    }

    const dtk::V2I& Viewport::getViewPos() const
    {
        return _viewPos->get();
    }

    float Viewport::getViewZoom() const
    {
        return _viewZoom->get();
    }

    std::shared_ptr<dtk::IObservableValue<dtk::V2I> > Viewport::observeViewPos() const
    {
        return _viewPos;
    }

    std::shared_ptr<dtk::IObservableValue<float> > Viewport::observeViewZoom() const
    {
        return _viewZoom;
    }

    void Viewport::setViewPosZoom(const dtk::V2I& pos, float zoom)
    {
        setFrameView(false);
        bool changed = _viewPos->setIfChanged(pos);
        changed |= _viewZoom->setIfChanged(zoom);
        if (changed)
        {
            _setDrawUpdate();
        }
    }

    void Viewport::setViewZoom(float value)
    {
        const dtk::Box2I& g = getGeometry();
        const dtk::V2I viewportCenter(g.w() / 2, g.h() / 2);
        setViewZoom(value, _isMouseInside() ? (_getMousePos() - g.min) : viewportCenter);
    }

    void Viewport::setViewZoom(float zoom, const dtk::V2I& focus)
    {
        dtk::V2I pos = _viewPos->get();
        const float zoomPrev = _viewZoom->get();
        pos.x = focus.x + (pos.x - focus.x) * (zoom / zoomPrev);
        pos.y = focus.y + (pos.y - focus.y) * (zoom / zoomPrev);
        setViewPosZoom(pos, zoom);
    }

    void Viewport::viewZoomIn(double amount)
    {
        setViewZoom(_viewZoom->get() * amount);
    }

    void Viewport::viewZoomOut(double amount)
    {
        setViewZoom(_viewZoom->get() / amount);
    }

    void Viewport::viewZoomReset()
    {
        setViewZoom(1.F);
    }

    bool Viewport::getFrameView() const
    {
        return _frameView->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > Viewport::observeFrameView() const
    {
        return _frameView;
    }

    void Viewport::setFrameView(bool value)
    {
        if (_frameView->setIfChanged(value))
        {
            _viewModel->setFrameView(value);
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
            if (_frameView->get())
            {
                _frameUpdate();
            }
            const dtk::Size2I& imageSize = _image->getSize();
            dtk::M44F vm;
            vm = vm * dtk::translate(dtk::V3F(g.min.x, g.min.y, 0.F));
            vm = vm * dtk::translate(dtk::V3F(_viewPos->get().x, _viewPos->get().y, 0.F));
            vm = vm * dtk::scale(dtk::V3F(_viewZoom->get(), _viewZoom->get(), 1.F));
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
            _viewPos->setIfChanged(dtk::V2I(
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
            setFrameView(false);
            _viewMousePress = _viewPos->get();
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
                viewZoomOut(.9F);
            }
            else if (event.value.y < 0)
            {
                viewZoomIn(.9F);
            }
        }
    }

    void Viewport::_frameUpdate()
    {
        if (_frameView->get() && _image)
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

            _viewPos->setIfChanged(pos);
            _viewZoom->setIfChanged(zoom);
        }
    }
}
