// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Comp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    CompNode::CompNode(const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Comp", inputs)
    {}

    CompNode::~CompNode()
    {}

    void CompNode::setPremult(bool premult)
    {
        _premult = premult;
    }

    void CompNode::setResize(bool resize)
    {
        _resize = resize;
    }

    OIIO::ImageBuf CompNode::exec()
    {
        OIIO::ImageBuf buf;
        OTIO_NS::RationalTime offsetTime = _time;
        if (!_timeOffset.is_invalid_time())
        {
            offsetTime -= _timeOffset;
        }
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            _inputs[0]->setTime(offsetTime);
            auto fg = _inputs[0]->exec();
            _inputs[1]->setTime(offsetTime);
            auto bg = _inputs[1]->exec();
            if (_premult)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            const auto& fgSpec = fg.spec();
            const auto& bgSpec = bg.spec();
            if (fgSpec.width != bgSpec.width ||
                fgSpec.height != bgSpec.height)
            {
                fg = OIIO::ImageBufAlgo::resize(
                    fg,
                    "",
                    0.0, 
                    OIIO::ROI(0, bgSpec.width, 0, bgSpec.height));
            }
            buf = OIIO::ImageBufAlgo::over(fg, bg);
        }
        else if (1 == _inputs.size() && _inputs[0])
        {
            _inputs[0]->setTime(offsetTime);
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
