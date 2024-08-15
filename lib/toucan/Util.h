// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <opentimelineio/anyVector.h>

#include <Imath/ImathVec.h>

#include <filesystem>
#include <string>
#include <utility>

namespace toucan
{
    //! Find plugins.
    void findPlugins(const std::filesystem::path&, std::vector<std::filesystem::path>&);

    //! Conversion to any vector.
    OTIO_NS::AnyVector vecToAny(const IMATH_NAMESPACE::V2i&);
    OTIO_NS::AnyVector vecToAny(const IMATH_NAMESPACE::V4f&);
    
    //! Conversion from any vector.
    void anyToVec(const OTIO_NS::AnyVector&, IMATH_NAMESPACE::V2i&);
    void anyToVec(const OTIO_NS::AnyVector&, IMATH_NAMESPACE::V4f&);
    
    //! Split the number from a file path stem.
    std::pair<std::string, std::string> splitFileNameNumber(const std::string&);

    //! Return the zero padding for the given number.
    size_t getNumberPadding(const std::string&);
}
