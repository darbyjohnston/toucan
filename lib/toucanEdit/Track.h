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
        Track(const std::string& kind);

        virtual ~Track();

        const std::string& getKind() const;

    private:
        std::string _kind;
    };
}

