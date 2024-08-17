// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Box.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    BoxNode::BoxNode(
        const BoxData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    BoxNode::~BoxNode()
    {}

    const BoxData& BoxNode::getData() const
    {
        return _data;
    }

    void BoxNode::setData(const BoxData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf BoxNode::exec(
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
            buf = _inputs[0]->exec(offsetTime, host);
            OIIO::ImageBufAlgo::render_box(
                buf,
                _data.pos1.x,
                _data.pos1.y,
                _data.pos2.x,
                _data.pos2.y,
                { _data.color.x, _data.color.y, _data.color.z, _data.color.w },
                _data.fill);
        }
        return buf;
    }
    
    BoxEffect::BoxEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    BoxEffect::~BoxEffect()
    {}

    std::shared_ptr<IImageNode> BoxEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<BoxNode>(_data, inputs);
    }

    bool BoxEffect::read_from(Reader& reader)
    {
        OTIO_NS::AnyVector pos1;
        OTIO_NS::AnyVector pos2;
        OTIO_NS::AnyVector color;
        bool out =
            reader.read("pos1", &pos1) &&
            reader.read("pos2", &pos2) &&
            reader.read("color", &color) &&
            reader.read("fill", &_data.fill) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(pos1, _data.pos1);
            anyToVec(pos2, _data.pos2);
            anyToVec(color, _data.color);
        }
        return out;
    }

    void BoxEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("pos1", vecToAny(_data.pos1));
        writer.write("pos2", vecToAny(_data.pos2));
        writer.write("color", vecToAny(_data.color));
        writer.write("fill", _data.fill);
    }
}
