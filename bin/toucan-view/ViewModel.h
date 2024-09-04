// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/core/ObservableValue.h>
#include <dtk/core/Vector.h>

namespace toucan
{
    class ViewModel : public std::enable_shared_from_this<ViewModel>
    {
    public:
        ViewModel();

        virtual ~ViewModel();

        void setViewportSize(const dtk::Size2I&);
        void setImageSize(const dtk::Size2I&);
        void setMouseInside(bool);
        void setMousePos(const dtk::V2I&);
        void setMousePress(bool);

        const dtk::V2I& getPos() const;
        float getZoom() const;
        std::shared_ptr<dtk::IObservableValue<dtk::V2I> > observePos() const;
        std::shared_ptr<dtk::IObservableValue<float> > observeZoom() const;
        void setPosZoom(const dtk::V2I&, float);
        void setZoom(float);
        void setZoom(float, const dtk::V2I& focus);
        void zoomIn(double amount = 2.F);
        void zoomOut(double amount = 2.F);
        void zoomReset();

        bool getFrame() const;
        std::shared_ptr<dtk::IObservableValue<bool> > observeFrame() const;
        void setFrame(bool);

    private:
        void _frameUpdate();

        dtk::Size2I _viewportSize;
        dtk::Size2I _imageSize;
        bool _mouseInside = false;
        dtk::V2I _mousePos;
        bool _mousePress = false;
        dtk::V2I _mousePressPos;
        dtk::V2I _mousePressView;
        std::shared_ptr<dtk::ObservableValue<dtk::V2I> > _pos;
        std::shared_ptr<dtk::ObservableValue<float> > _zoom;
        std::shared_ptr<dtk::ObservableValue<bool> > _frame;
    };
}
