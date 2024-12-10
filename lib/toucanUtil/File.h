// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <filesystem>
#include <string>

namespace toucan
{
    //! Find plugins.
    void findPlugins(const std::filesystem::path&, std::vector<std::filesystem::path>&);

    //! Split the URL protocol.
    std::pair<std::string, std::string> splitURLProtocol(const std::string&);

    //! Get an image sequence file name.
    std::string getSequenceFrame(
        const std::filesystem::path&,
        const std::string& namePrefix,
        int frame,
        int padding,
        const std::string& nameSuffix);

    //! Split the number from a file name.
    std::pair<std::string, std::string> splitFileNameNumber(const std::string&);

    //! Get the zero padding for the given number.
    size_t getNumberPadding(const std::string&);

    //! Make a unique temp directory.
    std::filesystem::path makeUniqueTemp();
}
