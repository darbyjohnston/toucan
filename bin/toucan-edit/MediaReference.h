// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <memory>
#include <filesystem>
#include <string>

namespace toucan
{
    class MediaReference : public std::enable_shared_from_this<MediaReference>
    {
    public:
        MediaReference();

        virtual ~MediaReference() = 0;
    };

    class ExternalReference : public MediaReference
    {
    public:
        ExternalReference();

        ~ExternalReference();

        const std::string& getURL() const;
        void setURL(const std::string&);

    private:
        std::string _url;
    };

    std::filesystem::path getMediaPath(
        const std::filesystem::path& timelinePath,
        const std::string& url);
}

