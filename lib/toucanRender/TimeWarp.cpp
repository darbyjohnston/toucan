// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TimeWarp.h"

namespace toucan
{
    LinearTimeWarpNode::LinearTimeWarpNode(
        double timeScalar,
        const OTIO_NS::TimeRange& timeRange,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("LinearTimeWarp", inputs),
        _timeScalar(timeScalar),
        _timeRange(timeRange)
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
            double timeScalar = _timeScalar;
            if (timeScalar < 0.0)
            {
                timeScalar *= -1.0;
                offsetTime = _timeRange.duration() -
                    OTIO_NS::RationalTime(1.0, _timeRange.duration().rate()) -
                    offsetTime;
            }
            const OTIO_NS::RationalTime scaledTime = OTIO_NS::RationalTime(offsetTime.value() * timeScalar, _time.rate()).floor();
            _inputs[0]->setTime(scaledTime);
            buf = _inputs[0]->exec();
        }
        return buf;
    }
}
