// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageOp.h"

namespace toucan
{
    IImageOp::IImageOp(
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        _inputs(inputs)
    {}

    IImageOp::~IImageOp()
    {}

    void IImageOp::setTimeOffset(const OTIO_NS::RationalTime& timeOffset)
    {
        _timeOffset = timeOffset;
    }

    IEffect::IEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        Effect(name, effect_name, metadata)
    {}

    IEffect::~IEffect()
    {}
}
