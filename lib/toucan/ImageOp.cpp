// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageOp.h"

namespace toucan
{
    IImageOp::IImageOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        _timeOffset(timeOffset),
        _inputs(inputs)
    {}

    IImageOp::~IImageOp()
    {}

    IEffect::IEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        Effect(name, effect_name, metadata)
    {}

    IEffect::~IEffect()
    {}
}
