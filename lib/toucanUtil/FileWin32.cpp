// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "File.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#include <combaseapi.h>

namespace toucan
{
    std::filesystem::path makeUniqueTemp()
    {
        std::string unique;
        GUID guid;
        CoCreateGuid(&guid);
        const uint8_t* guidP = reinterpret_cast<const uint8_t*>(&guid);
        for (int i = 0; i < 16; ++i)
        {
            char buf[3] = "";
            sprintf_s(buf, 3, "%02x", guidP[i]);
            unique += buf;
        }
        std::filesystem::path path = std::filesystem::temp_directory_path() / unique;
        std::filesystem::create_directory(path);
        return path;
    }
}
