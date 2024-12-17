// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

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
            buf = _inputs[1]->exec();
            const auto fgSpec = fg.spec();
            if (_premult &&
                fgSpec.width > 0 &&
                fgSpec.height > 0)
            {
                fg = OIIO::ImageBufAlgo::premult(fg);
            }
            const auto& bgSpec = buf.spec();
            if (fgSpec.width > 0 &&
                fgSpec.height > 0 &&
                bgSpec.width > 0 &&
                bgSpec.height > 0 &&
                (fgSpec.width != bgSpec.width || fgSpec.height != bgSpec.height))
            {
                fg = OIIO::ImageBufAlgo::resize(
                    fg,
                    "",
                    0.0, 
                    OIIO::ROI(0, bgSpec.width, 0, bgSpec.height));
            }
            if (fgSpec.width > 0 &&
                fgSpec.height > 0)
            {
                buf = OIIO::ImageBufAlgo::over(fg, buf);
            }
        }
        else if (1 == _inputs.size() && _inputs[0])
        {
            _inputs[0]->setTime(offsetTime);
            buf = _inputs[0]->exec();
            if (_premult)
            {
                buf = OIIO::ImageBufAlgo::premult(buf);
            }
        }
        return buf;
    }
}
