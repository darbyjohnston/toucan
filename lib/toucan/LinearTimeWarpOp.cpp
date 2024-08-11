// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "LinearTimeWarpOp.h"

namespace toucan
{
    LinearTimeWarpOp::LinearTimeWarpOp(
        float timeScalar,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(inputs),
        _timeScalar(timeScalar)
    {}

    LinearTimeWarpOp::~LinearTimeWarpOp()
    {}

    OIIO::ImageBuf LinearTimeWarpOp::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            const OTIO_NS::RationalTime scaledTime = OTIO_NS::RationalTime(offsetTime.value() * _timeScalar, offsetTime.rate()).floor();
            buf = _inputs[0]->exec(scaledTime);
        }
        return buf;
    }
}
