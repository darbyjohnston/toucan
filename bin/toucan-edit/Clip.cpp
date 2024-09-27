// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Clip.h"

#include "MediaReference.h"

namespace toucan
{
    Clip::Clip()
    {}

    Clip::~Clip()
    {}

    const Clip::MediaReferences& Clip::getMediaReferences() const
    {
        return _mediaReferences;
    }

    void Clip::setMediaReferences(const MediaReferences& value)
    {
        _mediaReferences = value;
    }

    const std::string Clip::defaultMediaKey = "DEFAULT_MEDIA";

    const std::string& Clip::getActiveMediaReference() const
    {
        return _activeMediaReference;
    }

    void Clip::setActiveMediaReference(const std::string& value)
    {
        _activeMediaReference = value;
    }
}
