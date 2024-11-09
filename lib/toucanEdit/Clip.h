// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanEdit/IItem.h>

#include <map>

namespace toucan
{
    class IMediaReference;

    class Clip : public IItem
    {
    public:
        Clip(
            const std::string& name = std::string(),
            const OTIO_NS::TimeRange& range = OTIO_NS::TimeRange(),
            const OTIO_NS::AnyDictionary& metadata = OTIO_NS::AnyDictionary());

        virtual ~Clip();

        using MediaReferences = std::map<std::string, std::shared_ptr<IMediaReference> >;

        const MediaReferences& getMediaReferences() const;
        std::shared_ptr<IMediaReference> getMediaReference() const;
        void setMediaReferences(const MediaReferences&);

        static const std::string defaultMediaKey;

        const std::string& getActiveMediaReference() const;
        void setActiveMediaReference(const std::string&);

    private:
        MediaReferences _mediaReferences;
        std::string _activeMediaReference = defaultMediaKey;
    };
}

