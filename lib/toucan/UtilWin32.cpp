// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Util.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>

namespace toucan
{
    std::string getLastError()
    {
        std::string out;
        const DWORD dw = GetLastError();
        if (dw != ERROR_SUCCESS)
        {
            const size_t bufSize = 4096;
            TCHAR buf[bufSize];
            FormatMessage(
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                buf,
                bufSize,
                NULL);
            out = std::string(buf, lstrlen(buf));
        }
        return out;
    }
}
