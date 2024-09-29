// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucanEdit/IContainer.h>

namespace toucan
{
    struct TrackKind
    {
        static const std::string video;
        static const std::string audio;
    };
    class Track : public IContainer
    {
    public:
        Track(
            const std::string& name = std::string(),
            const OTIO_NS::TimeRange& range = OTIO_NS::TimeRange(),
            const OTIO_NS::AnyDictionary& metadata = OTIO_NS::AnyDictionary(),
            const std::string& kind = TrackKind::video);

        virtual ~Track();

        const std::string& getKind() const;

    private:
        std::string _kind;
    };
}

