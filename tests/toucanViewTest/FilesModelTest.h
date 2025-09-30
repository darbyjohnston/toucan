// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/ImageEffectHost.h>

#include <ftk/Core/Context.h>

namespace toucan
{
    void filesModelTest(
        const std::shared_ptr<ftk::Context>&,
        const std::shared_ptr<ImageEffectHost>&,
        const std::filesystem::path& path);
}
