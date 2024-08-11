// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TransitionOp.h"

#include <OpenImageIO/imagebufalgo.h>

#include <iostream>

namespace toucan
{
    TransitionOp::TransitionOp(
        const OTIO_NS::TimeRange& range,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(inputs),
        _range(range)
    {}

    TransitionOp::~TransitionOp()
    {}

    OIIO::ImageBuf TransitionOp::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf buf;
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }

            const auto a = _inputs[0]->exec(offsetTime);
            const auto b = _inputs[1]->exec(offsetTime);
            
            const float v =
                (offsetTime.rescaled_to(_range.duration().rate()) - _range.start_time()).value() /
                _range.duration().value();

            buf = OIIO::ImageBufAlgo::add(
                OIIO::ImageBufAlgo::mul(
                    a,
                    OIIO::ImageBufAlgo::fill({ 1.F - v, 1.F - v, 1.F - v, 1.F - v }, a.roi())),
                OIIO::ImageBufAlgo::mul(
                    b,
                    OIIO::ImageBufAlgo::fill({ v, v, v, v }, b.roi())));
        }
        return buf;
    }
}
