// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageComp.h"

namespace toucan
{
    ImageComp::~ImageComp()
    {}

    void ImageComp::setInputs(const std::vector<std::shared_ptr<IImageGenerator> >& inputs)
    {
        _inputs = inputs;
    }

    OIIO::ImageBuf ImageComp::exec()
    {
        OIIO::ImageBuf buf;
        return buf;
    }
}
