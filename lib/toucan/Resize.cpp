// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Resize.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    ResizeNode::ResizeNode(
        const ResizeData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    ResizeNode::~ResizeNode()
    {}

    const ResizeData& ResizeNode::getData() const
    {
        return _data;
    }

    void ResizeNode::setData(const ResizeData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf ResizeNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<Host>& host)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty() && _inputs[0])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            buf = OIIO::ImageBufAlgo::resize(
                _inputs[0]->exec(offsetTime, host),
                _data.filterName,
                _data.filterWidth,
                OIIO::ROI(0, _data.size.x, 0, _data.size.y, 0, 1, 0, 4));
        }
        return buf;
    }
    
    ResizeEffect::ResizeEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    ResizeEffect::~ResizeEffect()
    {}

    std::shared_ptr<IImageNode> ResizeEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<ResizeNode>(_data, inputs);
    }

    bool ResizeEffect::read_from(Reader& reader)
    {
        int64_t width = 0;
        int64_t height = 0;
        double filterWidth = 0.0;
        bool out =
            reader.read("width", &width) &&
            reader.read("height", &height) &&
            reader.read("filter_name", &_data.filterName) &&
            reader.read("filter_width", &filterWidth) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.size.x = width;
            _data.size.y = height;
            _data.filterWidth = filterWidth;
        }
        return out;
    }

    void ResizeEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("width", static_cast<int64_t>(_data.size.x));
        writer.write("height", static_cast<int64_t>(_data.size.y));
        writer.write("filter_name", _data.filterName);
        writer.write("filter_width", static_cast<double>(_data.filterWidth));
    }
}
