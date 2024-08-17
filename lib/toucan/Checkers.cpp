// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Checkers.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    CheckersNode::CheckersNode(
        const CheckersData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    CheckersNode::~CheckersNode()
    {}

    const CheckersData& CheckersNode::getData() const
    {
        return _data;
    }

    void CheckersNode::setData(const CheckersData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf CheckersNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
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
            OIIO::ImageBufAlgo::checker(
                buf,
                _data.checkerSize.x,
                _data.checkerSize.y,
                1,
                { _data.color1.x, _data.color1.y, _data.color1.z, _data.color1.w },
                { _data.color2.x, _data.color2.y, _data.color2.z, _data.color2.w });
        }
        else
        {
            buf = OIIO::ImageBufAlgo::checker(
                _data.checkerSize.x,
                _data.checkerSize.y,
                1,
                { _data.color1.x, _data.color1.y, _data.color1.z, _data.color1.w },
                { _data.color2.x, _data.color2.y, _data.color2.z, _data.color2.w },
                0,
                0,
                0,
                OIIO::ROI(0, _data.size.x, 0, _data.size.y, 0, 1, 0, 4));
        }
        return buf;
    }
    
    CheckersEffect::CheckersEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    CheckersEffect::~CheckersEffect()
    {}

    std::shared_ptr<IImageNode> CheckersEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<CheckersNode>(_data, inputs);
    }

    bool CheckersEffect::read_from(Reader& reader)
    {
        //! \todo What is a better way to serialize non-POD types?
        OTIO_NS::AnyVector size;
        OTIO_NS::AnyVector checkerSize;
        OTIO_NS::AnyVector color1;
        OTIO_NS::AnyVector color2;
        bool out =
            reader.read("size", &size) &&
            reader.read("checkerSize", &checkerSize) &&
            reader.read("color1", &color1) &&
            reader.read("color2", &color2) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(size, _data.size);
            anyToVec(checkerSize, _data.checkerSize);
            anyToVec(color1, _data.color1);
            anyToVec(color2, _data.color2);
        }
        return out;
    }

    void CheckersEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("size", vecToAny(_data.size));
        writer.write("checkerSize", vecToAny(_data.checkerSize));
        writer.write("color1", vecToAny(_data.color1));
        writer.write("color2", vecToAny(_data.color2));
    }
}
