// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    CompOp::~CompOp()
    {}

    void CompOp::setPremult(bool premult)
    {
        _premult = premult;
    }

    OIIO::ImageBuf CompOp::exec()
    {
        OIIO::ImageBuf buf;
        if (_inputs.size() > 1)
        {
            auto fg = _inputs[0]->exec();
            auto bg = _inputs[1]->exec();
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = OIIO::ImageBufAlgo::over(fg, bg);
        }
        else if (1 == _inputs.size())
        {
            auto fg = _inputs[0]->exec();
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = fg;
        }
        return buf;
    }
}
