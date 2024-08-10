// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Compositing image operation.
    class CompImageOp : public IImageOp
    {
    public:
        virtual ~CompImageOp();

        void setPremult(bool);

        OIIO::ImageBuf exec() override;

    private:
        bool _premult = false;
    };
}
