// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "File.h"

#include <stdlib.h>
#include <unistd.h>

namespace toucan
{
    std::filesystem::path makeUniqueTemp()
    {
        const std::filesystem::path path = std::filesystem::temp_directory_path() / "XXXXXX";
        std::vector<char> buf;
        for (const char c : path.string())
        {
            buf.push_back(c);
        }
        buf.push_back(0);
        return mkdtemp(buf.data());
    }
}
