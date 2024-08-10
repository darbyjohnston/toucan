// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageFill.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    ImageFill::~ImageFill()
    {}

    void ImageFill::setSize(int width, int height)
    {
        _width = width;
        _height = height;
    }

    void ImageFill::setColor(float r, float g, float b, float a)
    {
        _r = r;
        _g = g;
        _b = b;
        _a = a;
    }

    OIIO::ImageBuf ImageFill::exec()
    {
        OIIO::ImageBuf buf = OIIO::ImageBufAlgo::fill(
            { _r, _g, _b, _a },
            OIIO::ROI(0, _width, 0, _height, 0, 1, 0, 4));
        return buf;
    }
}
