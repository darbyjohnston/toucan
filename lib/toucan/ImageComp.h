// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Image compositing.
    class ImageComp : public IImageOpMulti
    {
    public:
        virtual ~ImageComp();

        void setInputs(const std::vector<std::shared_ptr<IImageOp> >&) override;

        void setPremult(bool);

        OIIO::ImageBuf exec() override;

    private:
        std::vector<std::shared_ptr<IImageOp> > _inputs;
        bool _premult = false;
    };
}
