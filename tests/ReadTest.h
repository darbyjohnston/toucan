// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageEffectHost.h>

namespace toucan
{
    void readTest(
        const std::filesystem::path&,
        const std::shared_ptr<ImageEffectHost>&);
}
