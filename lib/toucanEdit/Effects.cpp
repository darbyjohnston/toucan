// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Effects.h"

namespace toucan
{
    Effect::Effect(
        const std::string& name,
        const std::string& effectName,
        const OTIO_NS::AnyDictionary& metadata) :
        _name(name),
        _effectName(effectName),
        _metadata(metadata)
    {}

    Effect::~Effect()
    {}

    const std::string& Effect::getName() const
    {
        return _name;
    }

    void Effect::setName(const std::string& value)
    {
        _name = value;
    }

    const std::string& Effect::getEffectName() const
    {
        return _effectName;
    }

    void Effect::setEffectName(const std::string& value)
    {
        _effectName = value;
    }

    const OTIO_NS::AnyDictionary& Effect::getMetadata() const
    {
        return _metadata;
    }

    void Effect::setMetadata(const OTIO_NS::AnyDictionary& value)
    {
        _metadata = value;
    }

    LinearTimeWarp::LinearTimeWarp(
        const std::string & name,
        const std::string & effectName,
        const OTIO_NS::AnyDictionary & metadata) :
        Effect(name, effectName, metadata)
    {}

    LinearTimeWarp::~LinearTimeWarp()
    {}

    double LinearTimeWarp::getTimeScalar() const
    {
        return _timeScalar;
    }

    void LinearTimeWarp::setTimeScalar(double value)
    {
        _timeScalar = value;
    }
}
