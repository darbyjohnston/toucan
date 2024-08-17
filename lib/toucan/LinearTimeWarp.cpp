// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "LinearTimeWarp.h"

namespace toucan
{
    LinearTimeWarpNode::LinearTimeWarpNode(
        float timeScalar,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _timeScalar(timeScalar)
    {}

    LinearTimeWarpNode::~LinearTimeWarpNode()
    {}

    OIIO::ImageBuf LinearTimeWarpNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
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
            buf = _inputs[0]->exec(scaledTime, host);
        }
        return buf;
    }
}
