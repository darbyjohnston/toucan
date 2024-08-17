// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Invert.h"

namespace toucan
{
    InvertNode::InvertNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs)
    {}

    InvertNode::~InvertNode()
    {}

    OIIO::ImageBuf InvertNode::exec(
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
            const auto input = _inputs[0]->exec(offsetTime, host);
            const auto& spec = input.spec();
            buf = OIIO::ImageBuf(spec);
            host->filter("Toucan:Invert", input, buf);
        }
        return buf;
    }
    
    InvertEffect::InvertEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    InvertEffect::~InvertEffect()
    {}

    std::shared_ptr<IImageNode> InvertEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<InvertNode>(inputs);
    }

    bool InvertEffect::read_from(Reader& reader)
    {
        return IEffect::read_from(reader);
    }

    void InvertEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
    }
}
