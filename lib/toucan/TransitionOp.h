// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Transition operation.
    class TransitionOp : public IImageOp
    {
    public:
        TransitionOp(
            const OTIO_NS::TimeRange&,
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~TransitionOp();
        
        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        OTIO_NS::TimeRange _range;
    };
}
