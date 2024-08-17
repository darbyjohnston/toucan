// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Saturate.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    SaturateNode::SaturateNode(
        const SaturateData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    SaturateNode::~SaturateNode()
    {}

    const SaturateData& SaturateNode::getData() const
    {
        return _data;
    }

    void SaturateNode::setData(const SaturateData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf SaturateNode::exec(
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
            buf = OIIO::ImageBufAlgo::saturate(
                _inputs[0]->exec(offsetTime, host),
                _data.value);
        }
        return buf;
    }
    
    SaturateEffect::SaturateEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    SaturateEffect::~SaturateEffect()
    {}

    std::shared_ptr<IImageNode> SaturateEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<SaturateNode>(_data, inputs);
    }

    bool SaturateEffect::read_from(Reader& reader)
    {
        double value = 0.0;
        bool out =
            reader.read("value", &value) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.value = value;
        }
        return out;
    }

    void SaturateEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("value", static_cast<double>(_data.value));
    }
}
