// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "IItem.h"

#include <map>

namespace toucan
{
    class MediaReference;

    class Clip : public IItem
    {
    public:
        Clip();

        virtual ~Clip();

        using MediaReferences = std::map<std::string, std::shared_ptr<MediaReference> >;

        const MediaReferences& getMediaReferences() const;
        void setMediaReferences(const MediaReferences&);

        static const std::string defaultMediaKey;

        const std::string& getActiveMediaReference() const;
        void setActiveMediaReference(const std::string&);

    private:
        MediaReferences _mediaReferences;
        std::string _activeMediaReference = defaultMediaKey;
    };
}

