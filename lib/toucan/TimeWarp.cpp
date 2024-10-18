// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimeWarp.h"

namespace toucan
{
    LinearTimeWarpNode::LinearTimeWarpNode(
        float timeScalar,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("LinearTimeWarp", inputs),
        _timeScalar(timeScalar)
    {}

    LinearTimeWarpNode::~LinearTimeWarpNode()
    {}

    OIIO::ImageBuf LinearTimeWarpNode::exec()
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            OTIO_NS::RationalTime offsetTime = _time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            const OTIO_NS::RationalTime scaledTime = OTIO_NS::RationalTime(offsetTime.value() * _timeScalar, offsetTime.rate()).floor();
            _inputs[0]->setTime(scaledTime);
            buf = _inputs[0]->exec();
        }
        return buf;
    }
}
