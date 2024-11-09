// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "Clip.h"

#include "MediaReferences.h"

namespace toucan
{
    Clip::Clip(
        const std::string& name,
        const OTIO_NS::TimeRange& range,
        const OTIO_NS::AnyDictionary& metadata) :
        IItem(name, range, metadata)
    {}

    Clip::~Clip()
    {}

    const Clip::MediaReferences& Clip::getMediaReferences() const
    {
        return _mediaReferences;
    }

    std::shared_ptr<IMediaReference> Clip::getMediaReference() const
    {
        std::shared_ptr<IMediaReference> out;
        auto i = _mediaReferences.find(_activeMediaReference);
        return i != _mediaReferences.end() ? i->second : nullptr;
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
