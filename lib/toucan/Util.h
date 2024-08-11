// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <string>
#include <utility>

namespace toucan
{
    //! Split the number from a file path stem.
    std::pair<std::string, std::string> splitFileNameNumber(const std::string&);

    //! Return the zero padding for the given number.
    size_t getNumberPadding(const std::string&);
}
