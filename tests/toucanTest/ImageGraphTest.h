// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucan/ImageEffectHost.h>

namespace toucan
{
    void imageGraphTest(
        const std::filesystem::path&,
        const std::shared_ptr<ImageEffectHost>&);
}
