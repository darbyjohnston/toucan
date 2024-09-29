// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucanEdit/IItem.h>

#include <memory>

namespace toucan
{
    class Transition : public IItem
    {
    public:
        Transition(
            const std::string& name = std::string(),
            const OTIO_NS::TimeRange& range = OTIO_NS::TimeRange(),
            const OTIO_NS::AnyDictionary& metadata = OTIO_NS::AnyDictionary(),
            const std::string& transitionType = std::string(),
            const OTIO_NS::RationalTime& inOffset = OTIO_NS::RationalTime(),
            const OTIO_NS::RationalTime& outOffset = OTIO_NS::RationalTime());

        virtual ~Transition();

        const std::string& getTransitionType() const;
        void setTransitionType(const std::string&);

        const OTIO_NS::RationalTime& getInOffset() const;
        void setInOffset(const OTIO_NS::RationalTime&);

        const OTIO_NS::RationalTime& getOutOffset() const;
        void setOutOffset(const OTIO_NS::RationalTime&);

    private:
        std::string _transitionType;
        OTIO_NS::RationalTime _inOffset;
        OTIO_NS::RationalTime _outOffset;
    };
}

