// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ViewModel.h"

namespace toucan
{
    ViewModel::ViewModel()
    {
        _pos = dtk::ObservableValue<dtk::V2I>::create();
        _zoom = dtk::ObservableValue<float>::create(1.F);
        _frame = dtk::ObservableValue<bool>::create(true);
    }

    ViewModel::~ViewModel()
    {}

    void ViewModel::setViewportSize(const dtk::Size2I & value)
    {
        if (value == _viewportSize)
            return;
        _viewportSize = value;
        _frameUpdate();
    }

    void ViewModel::setImageSize(const dtk::Size2I& value)
    {
        if (value == _imageSize)
            return;
        _imageSize = value;
        _frameUpdate();
    }

    void ViewModel::setMouseInside(bool value)
    {
        _mouseInside = value;
    }

    void ViewModel::setMousePos(const dtk::V2I& value)
    {
        _mousePos = value;
        if (_mousePress)
        {
            dtk::V2I pos;
            pos.x = _mousePressView.x + (_mousePos.x - _mousePressPos.x);
            pos.y = _mousePressView.y + (_mousePos.y - _mousePressPos.y);
            _pos->setIfChanged(pos);
        }
    }

    void ViewModel::setMousePress(bool value)
    {
        _mousePress = value;
        if (value)
        {
            _mousePressPos = _mousePos;
            _mousePressView = _pos->get();
        }
    }

    const dtk::V2I& ViewModel::getPos() const
    {
        return _pos->get();
    }

    float ViewModel::getZoom() const
    {
        return _zoom->get();
    }

    std::shared_ptr<dtk::IObservableValue<dtk::V2I> > ViewModel::observePos() const
    {
        return _pos;
    }

    std::shared_ptr<dtk::IObservableValue<float> > ViewModel::observeZoom() const
    {
        return _zoom;
    }

    void ViewModel::setPosZoom(const dtk::V2I& pos, float zoom)
    {
        _frame->setIfChanged(false);
        _pos->setIfChanged(pos);
        _zoom->setIfChanged(zoom);
    }

    void ViewModel::setZoom(float value)
    {
        const dtk::V2I viewportCenter(_viewportSize.w / 2, _viewportSize.h / 2);
        setZoom(value, _mouseInside ? _mousePos : viewportCenter);
    }

    void ViewModel::setZoom(float zoom, const dtk::V2I& focus)
    {
        dtk::V2I pos = _pos->get();
        const float zoomPrev = _zoom->get();
        pos.x = focus.x + (pos.x - focus.x) * (zoom / zoomPrev);
        pos.y = focus.y + (pos.y - focus.y) * (zoom / zoomPrev);
        setPosZoom(pos, zoom);
    }

    void ViewModel::zoomIn(double amount)
    {
        setZoom(_zoom->get() * amount);
    }

    void ViewModel::zoomOut(double amount)
    {
        setZoom(_zoom->get() / amount);
    }

    void ViewModel::zoomReset()
    {
        setZoom(1.F);
    }

    bool ViewModel::getFrame() const
    {
        return _frame->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeFrame() const
    {
        return _frame;
    }

    void ViewModel::setFrame(bool value)
    {
        if (_frame->setIfChanged(value))
        {
            _frameUpdate();
        }
    }

    void ViewModel::_frameUpdate()
    {
        if (_frame)
        {
            float zoom = _viewportSize.w / static_cast<float>(_imageSize.w);
            if (zoom * _imageSize.h > _viewportSize.h)
            {
                zoom = _viewportSize.h / static_cast<double>(_imageSize.h);
            }
            const dtk::V2I c(_imageSize.w / 2, _imageSize.h / 2);
            const dtk::V2I pos = dtk::V2I(
                _viewportSize.w / 2.F - c.x * zoom,
                _viewportSize.h / 2.F - c.y * zoom);

            _pos->setIfChanged(pos);
            _zoom->setIfChanged(zoom);
        }
    }
}
