// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    CompOp::CompOp(const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(inputs)
    {}

    CompOp::~CompOp()
    {}

    void CompOp::setPremult(bool premult)
    {
        _premult = premult;
    }

    OIIO::ImageBuf CompOp::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf buf;
        OTIO_NS::RationalTime offsetTime = time;
        if (!_timeOffset.is_invalid_time())
        {
            offsetTime -= _timeOffset;
        }
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            auto fg = _inputs[0]->exec(offsetTime);
            auto bg = _inputs[1]->exec(offsetTime);
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = OIIO::ImageBufAlgo::over(fg, bg);
        }
        else if (1 == _inputs.size() && _inputs[0])
        {
            auto fg = _inputs[0]->exec(offsetTime);
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = fg;
        }
        return buf;
    }
}
