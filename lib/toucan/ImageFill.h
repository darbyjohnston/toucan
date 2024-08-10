// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <filesystem>

namespace toucan
{
    //! Image fill.
    class ImageFill : public IImageOp
    {
    public:
        virtual ~ImageFill();

        void setSize(int width, int height);

        void setColor(float r, float g, float b, float a);
        
        OIIO::ImageBuf exec() override;

    private:
        int _width = 0;
        int _height = 0;
        float _r = 0.F;
        float _g = 0.F;
        float _b = 0.F;
        float _a = 0.F;
    };
}
