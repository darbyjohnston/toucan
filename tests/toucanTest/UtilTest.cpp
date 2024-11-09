// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "UtilTest.h"

#include <toucan/Util.h>

#include <cassert>

namespace toucan
{
    void utilTest(const std::filesystem::path& path)
    {
        {
            const auto split = splitFileNameNumber("");
            assert(split.first.empty());
            assert(split.second.empty());
        }
        {
            const auto split = splitFileNameNumber("0000");
            assert(split.first.empty());
            assert("0000" == split.second);
        }
        {
            const auto split = splitFileNameNumber("render0000");
            assert("render" == split.first);
            assert("0000" == split.second);
        }
        {
            const auto split = splitFileNameNumber("render.0000");
            assert("render." == split.first);
            assert("0000" == split.second);
        }
        {
            const int padding = getNumberPadding("");
            assert(0 == padding);
        }
        {
            const int padding = getNumberPadding("1234");
            assert(0 == padding);
        }
        {
            const int padding = getNumberPadding("0001");
            assert(4 == padding);
        }
    }
}
