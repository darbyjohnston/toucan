// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Transitions.h"

namespace toucan
{
    Transition::Transition(
        const std::string& name,
        const OTIO_NS::TimeRange& range,
        const OTIO_NS::AnyDictionary& metadata,
        const std::string& transitionType,
        const OTIO_NS::RationalTime& inOffset,
        const OTIO_NS::RationalTime& outOffset) :
        IItem(name, range, metadata),
        _transitionType(transitionType),
        _inOffset(inOffset),
        _outOffset(outOffset)
    {}

    Transition::~Transition()
    {}

    const std::string& Transition::getTransitionType() const
    {
        return _transitionType;
    }

    void Transition::setTransitionType(const std::string& value)
    {
        _transitionType = value;
    }

    const OTIO_NS::RationalTime& Transition::getInOffset() const
    {
        return _inOffset;
    }

    void Transition::setInOffset(const OTIO_NS::RationalTime& value)
    {
        _inOffset = value;
    }

    const OTIO_NS::RationalTime& Transition::getOutOffset() const
    {
        return _outOffset;
    }

    void Transition::setOutOffset(const OTIO_NS::RationalTime& value)
    {
        _outOffset = value;
    }
}
