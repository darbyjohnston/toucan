// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Rotate.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    RotateNode::RotateNode(
        const RotateData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    RotateNode::~RotateNode()
    {}

    const RotateData& RotateNode::getData() const
    {
        return _data;
    }

    void RotateNode::setData(const RotateData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf RotateNode::exec(
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
            buf = OIIO::ImageBufAlgo::rotate(
                _inputs[0]->exec(offsetTime, host),
                _data.angle / 360.F * 2.F * M_PI,
                _data.filterName,
                _data.filterWidth);
        }
        return buf;
    }
    
    RotateEffect::RotateEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    RotateEffect::~RotateEffect()
    {}

    std::shared_ptr<IImageNode> RotateEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<RotateNode>(_data, inputs);
    }

    bool RotateEffect::read_from(Reader& reader)
    {
        double angle = 0.0;
        double filterWidth = 0.0;
        bool out =
            reader.read("angle", &angle) &&
            reader.read("filter_name", &_data.filterName) &&
            reader.read("filter_width", &filterWidth) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.angle = angle;
            _data.filterWidth = filterWidth;
        }
        return out;
    }

    void RotateEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("angle", static_cast<double>(_data.angle));
        writer.write("filter_name", _data.filterName);
        writer.write("filter_width", static_cast<double>(_data.filterWidth));
    }
}
