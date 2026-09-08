// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <cstddef>
#include <vector>

namespace toucan
{
    struct AudioBuffer
    {
        std::vector<float> data;
        int sampleRate = 0;
        int channelCount = 0;
        int sampleCount = 0;

        bool isValid() const { return !data.empty() && sampleRate > 0; }
        size_t byteCount() const { return data.size() * sizeof(float); }
    };
}
