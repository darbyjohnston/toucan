// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <opentimelineio/anyDictionary.h>

#include <memory>

namespace toucan
{
    class Effect : public std::enable_shared_from_this<Effect>
    {
    public:
        Effect(
            const std::string& name = std::string(),
            const std::string& effectName = std::string(),
            const OTIO_NS::AnyDictionary& metadata = OTIO_NS::AnyDictionary());

        virtual ~Effect();

        const std::string& getName() const;
        void setName(const std::string&);

        const std::string& getEffectName() const;
        void setEffectName(const std::string&);

        const OTIO_NS::AnyDictionary& getMetadata() const;
        void setMetadata(const OTIO_NS::AnyDictionary&);

    private:
        std::string _name;
        std::string _effectName;
        OTIO_NS::AnyDictionary _metadata;
    };

    class LinearTimeWarp : public Effect
    {
    public:
        LinearTimeWarp(
            const std::string& name = std::string(),
            const std::string& effectName = std::string(),
            const OTIO_NS::AnyDictionary& metadata = OTIO_NS::AnyDictionary());

        virtual ~LinearTimeWarp();

        double getTimeScalar() const;
        void setTimeScalar(double);

    private:
        double _timeScalar = 1.0;
    };
}

