// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "FlipOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    FlipOp::FlipOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(timeOffset, inputs)
    {}

    FlipOp::~FlipOp()
    {}

    OIIO::ImageBuf FlipOp::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            const auto input = _inputs[0]->exec(time);
            buf = OIIO::ImageBufAlgo::flip(input);
        }
        return buf;
    }
    
    FlipEffect::FlipEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    FlipEffect::~FlipEffect()
    {}

    std::shared_ptr<IImageOp> FlipEffect::createOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs)
    {
        return std::make_shared<FlipOp>(timeOffset, inputs);
    }

    bool FlipEffect::read_from(Reader& reader)
    {
        return IEffect::read_from(reader);
    }

    void FlipEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
    }
}
