// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    CompOp::CompOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(timeOffset, inputs)
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
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            auto fg = _inputs[0]->exec(time);
            auto bg = _inputs[1]->exec(time);
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = OIIO::ImageBufAlgo::over(fg, bg);
        }
        else if (1 == _inputs.size() && _inputs[0])
        {
            auto fg = _inputs[0]->exec(time);
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = fg;
        }
        return buf;
    }
}
