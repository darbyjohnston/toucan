// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "FlopOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    FlopOp::FlopOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(timeOffset, inputs)
    {}

    FlopOp::~FlopOp()
    {}

    OIIO::ImageBuf FlopOp::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            const auto input = _inputs[0]->exec(time);
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

    std::shared_ptr<IImageOp> FlopEffect::createOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs)
    {
        return std::make_shared<FlopOp>(timeOffset, inputs);
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
