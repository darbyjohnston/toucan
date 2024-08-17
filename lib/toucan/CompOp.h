// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Compositing operation.
    class CompOp : public IImageOp
    {
    public:
        CompOp(const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~CompOp();

        //! Set whether images are pre-multiplied before compositing.
        void setPremult(bool);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;

    private:
        bool _premult = false;
    };
}
