// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "InvertOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    InvertOp::InvertOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(timeOffset, inputs)
    {}

    InvertOp::~InvertOp()
    {}

    OIIO::ImageBuf InvertOp::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            const auto input = _inputs[0]->exec(time);
            auto roi = input.roi();
            roi.chend = 3;
            buf = OIIO::ImageBufAlgo::unpremult(input);
            OIIO::ImageBufAlgo::invert(buf, buf, roi);
            OIIO::ImageBufAlgo::repremult(buf, buf);
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

    std::shared_ptr<IImageOp> InvertEffect::createOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs)
    {
        return std::make_shared<InvertOp>(timeOffset, inputs);
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
