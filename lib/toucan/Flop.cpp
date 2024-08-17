// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Flop.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    FlopNode::FlopNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs)
    {}

    FlopNode::~FlopNode()
    {}

    OIIO::ImageBuf FlopNode::exec(
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
            const auto input = _inputs[0]->exec(offsetTime, host);
            buf = OIIO::ImageBufAlgo::flop(input);
        }
        return buf;
    }
    
    FlopEffect::FlopEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    FlopEffect::~FlopEffect()
    {}

    std::shared_ptr<IImageNode> FlopEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<FlopNode>(inputs);
    }

    bool FlopEffect::read_from(Reader& reader)
    {
        return IEffect::read_from(reader);
    }

    void FlopEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
    }
}
