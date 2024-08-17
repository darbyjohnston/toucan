// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Comp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    CompNode::CompNode(const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs)
    {}

    CompNode::~CompNode()
    {}

    void CompNode::setPremult(bool premult)
    {
        _premult = premult;
    }

    OIIO::ImageBuf CompNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OIIO::ImageBuf buf;
        OTIO_NS::RationalTime offsetTime = time;
        if (!_timeOffset.is_invalid_time())
        {
            offsetTime -= _timeOffset;
        }
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            auto fg = _inputs[0]->exec(offsetTime, host);
            auto bg = _inputs[1]->exec(offsetTime, host);
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = OIIO::ImageBufAlgo::over(fg, bg);
        }
        else if (1 == _inputs.size() && _inputs[0])
        {
            auto fg = _inputs[0]->exec(offsetTime, host);
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            buf = fg;
        }
        return buf;
    }
}
