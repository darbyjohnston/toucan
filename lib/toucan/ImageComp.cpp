// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageComp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    ImageComp::~ImageComp()
    {}

    void ImageComp::setInputs(const std::vector<std::shared_ptr<IImageGenerator> >& inputs)
    {
        _inputs = inputs;
    }

    void ImageComp::setPremult(bool premult)
    {
        _premult = premult;
    }

    OIIO::ImageBuf ImageComp::exec()
    {
        OIIO::ImageBuf buf;
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            auto fg = _inputs[0]->exec();
            auto bg = _inputs[1]->exec();
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = OIIO::ImageBufAlgo::over(fg, bg);
        }
        return buf;
    }
}
