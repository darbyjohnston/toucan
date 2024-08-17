// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageNode.h"

namespace toucan
{
    IImageNode::IImageNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        _inputs(inputs)
    {}

    IImageNode::~IImageNode()
    {}

    void IImageNode::setTimeOffset(const OTIO_NS::RationalTime& timeOffset)
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
