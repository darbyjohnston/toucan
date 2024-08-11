// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "InvertOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    InvertOp::InvertOp()
    {}

    InvertOp::~InvertOp()
    {}

    OIIO::ImageBuf InvertOp::exec()
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            const auto input = _inputs[0]->exec();
            const auto spec = input.spec();
            //! \todo The ROI is not working?
            buf = OIIO::ImageBufAlgo::invert(
                input);
                //OIIO::ROI(0, spec.width, 0, spec.height, 0, 1, 0, 3));
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

    std::shared_ptr<IImageOp> InvertEffect::createOp()
    {
        return std::make_shared<InvertOp>();
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
