// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "MediaReference.h"

#include <toucan/Util.h>

namespace toucan
{
    MediaReference::MediaReference()
    {}

    MediaReference::~MediaReference()
    {}

    ExternalReference::ExternalReference()
    {}

    ExternalReference::~ExternalReference()
    {}

    const std::string& ExternalReference::getURL() const
    {
        return _url;
    }

    void ExternalReference::setURL(const std::string& url)
    {
        _url = url;
    }

    std::filesystem::path getMediaPath(
        const std::filesystem::path& timelinePath,
        const std::string& url)
    {
        std::filesystem::path path = splitURLProtocol(url).second;
        if (!path.is_absolute())
        {
            path = timelinePath / path;
        }
        return path;
    }
}
