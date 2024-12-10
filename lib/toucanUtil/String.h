// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <string>
#include <vector>

namespace toucan
{
    //! Convert a string to lowercase.
    std::string toLower(std::string);

    //! Join a list of strings.
    std::string join(const std::vector<std::string>&, char delimeter);

    //! Join a list of strings.
    std::string join(const std::vector<std::string>&, const std::string& delimeter);
}
