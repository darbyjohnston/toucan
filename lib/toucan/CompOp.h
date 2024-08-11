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
        CompOp(
            const OTIO_NS::RationalTime& = OTIO_NS::RationalTime(),
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~CompOp();

        void setPremult(bool);

        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        bool _premult = false;
    };
}
