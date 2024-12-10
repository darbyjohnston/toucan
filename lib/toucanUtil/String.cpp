// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "String.h"

#include <algorithm>

namespace toucan
{
    std::string toLower(std::string value)
    {
        std::transform(
            value.begin(),
            value.end(),
            value.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return value;
    }

    std::string join(const std::vector<std::string>& values, char delimeter)
    {
        std::string out;
        const std::size_t size = values.size();
        for (std::size_t i = 0; i < size; ++i)
        {
            out += values[i];
            if (i < size - 1)
            {
                out += delimeter;
            }
        }
        return out;
    }

    std::string join(const std::vector<std::string>& values, const std::string& delimeter)
    {
        std::string out;
        const std::size_t size = values.size();
        for (std::size_t i = 0; i < size; ++i)
        {
            out += values[i];
            if (i < size - 1)
            {
                out += delimeter;
            }
        }
        return out;
    }
}
