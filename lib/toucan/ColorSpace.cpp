// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ColorSpace.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    PremultNode::PremultNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Premult", inputs)
    {}

    PremultNode::~PremultNode()
    {}

    OIIO::ImageBuf PremultNode::exec(
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
            const auto& spec = input.spec();
            buf = OIIO::ImageBuf(spec);
            host->filter("Toucan:Premult", input, buf);
        }
        return buf;
    }

    PremultEffect::PremultEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    PremultEffect::~PremultEffect()
    {}

    std::shared_ptr<IImageNode> PremultEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<PremultNode>(inputs);
    }

    UnpremultNode::UnpremultNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Unpremult", inputs)
    {}

    UnpremultNode::~UnpremultNode()
    {}

    OIIO::ImageBuf UnpremultNode::exec(
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
            const auto& spec = input.spec();
            buf = OIIO::ImageBuf(spec);
            host->filter("Toucan:Unpremult", input, buf);
        }
        return buf;
    }

    UnpremultEffect::UnpremultEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    UnpremultEffect::~UnpremultEffect()
    {}

    std::shared_ptr<IImageNode> UnpremultEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<UnpremultNode>(inputs);
    }
}
