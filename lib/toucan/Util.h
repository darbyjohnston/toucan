// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucan/PropertySet.h>

#include <OpenFX/ofxProperty.h>

#include <opentimelineio/anyVector.h>

#include <Imath/ImathVec.h>

#include <filesystem>
#include <string>
#include <utility>

namespace toucan
{
    //! Convert a string to lowercase.
    std::string toLower(std::string);

    //! Find plugins.
    void findPlugins(const std::filesystem::path&, std::vector<std::filesystem::path>&);

    //! Conversion to any vector.
    OTIO_NS::AnyVector vecToAny(const IMATH_NAMESPACE::V2i&);
    OTIO_NS::AnyVector vecToAny(const IMATH_NAMESPACE::V4f&);
    
    //! Conversion from any vector.
    void anyToVec(const OTIO_NS::AnyVector&, IMATH_NAMESPACE::V2i&);
    void anyToVec(const OTIO_NS::AnyVector&, IMATH_NAMESPACE::V4f&);

    //! Split the URL protocol.
    std::pair<std::string, std::string> splitURLProtocol(const std::string&);

    //! Join a list of strings.
    std::string join(const std::vector<std::string>&, char delimeter);

    //! Join a list of strings.
    std::string join(const std::vector<std::string>&, const std::string& delimeter);

    //! Get an image sequence file name.
    std::string getSequenceFrame(
        const std::filesystem::path&,
        const std::string& namePrefix,
        int frame,
        int padding,
        const std::string& nameSuffix);

    //! Split the number from a file path stem.
    std::pair<std::string, std::string> splitFileNameNumber(const std::string&);

    //! Return the zero padding for the given number.
    size_t getNumberPadding(const std::string&);

    //! Convert a floating point rate to a rational.
    std::pair<int, int> toRational(double);

    //! Make a unique temp directory.
    std::filesystem::path makeUniqueTemp();

#if defined(_WINDOWS)
    //! Get an error string from a Windows system call.
    std::string getLastError();
#endif // _WINDOWS
}
