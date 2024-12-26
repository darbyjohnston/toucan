// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/core/Context.h>

#include <filesystem>

namespace toucan
{
    void selectionModelTest(
        const std::shared_ptr<dtk::Context>&,
        const std::filesystem::path& path);
}
