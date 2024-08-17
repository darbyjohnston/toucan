// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Linear time warp operation.
    class LinearTimeWarpOp : public IImageOp
    {
    public:
        LinearTimeWarpOp(
            float timeScalar,
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~LinearTimeWarpOp();
        
        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;

    private:
        float _timeScalar = 1.F;
    };
}
