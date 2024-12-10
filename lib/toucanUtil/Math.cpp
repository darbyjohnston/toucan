// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "Math.h"

#include <array>

namespace toucan
{
    OTIO_NS::AnyVector vecToAny(const IMATH_NAMESPACE::V2i& vec)
    {
        return OTIO_NS::AnyVector
        {
            static_cast<int64_t>(vec.x),
            static_cast<int64_t>(vec.y)
        };
    }
    
    OTIO_NS::AnyVector vecToAny(const IMATH_NAMESPACE::V4f& vec)
    {
        return OTIO_NS::AnyVector
        {
            static_cast<double>(vec.x),
            static_cast<double>(vec.y),
            static_cast<double>(vec.z),
            static_cast<double>(vec.w)
        };
    }
    
    void anyToVec(const OTIO_NS::AnyVector& any, IMATH_NAMESPACE::V2i& out)
    {
        if (2 == any.size())
        {
            out.x = std::any_cast<int64_t>(any[0]);
            out.y = std::any_cast<int64_t>(any[1]);
        }
    }
    
    void anyToVec(const OTIO_NS::AnyVector& any, IMATH_NAMESPACE::V4f& out)
    {
        if (4 == any.size())
        {
            out.x = std::any_cast<double>(any[0]);
            out.y = std::any_cast<double>(any[1]);
            out.z = std::any_cast<double>(any[2]);
            out.w = std::any_cast<double>(any[3]);
        }
    }

    std::pair<int, int> toRational(double value)
    {
        const std::array<std::pair<int, int>, 6> common =
        {
            std::make_pair(24, 1),
            std::make_pair(30, 1),
            std::make_pair(60, 1),
            std::make_pair(24000, 1001),
            std::make_pair(30000, 1001),
            std::make_pair(60000, 1001)
        };
        const double tolerance = 0.01;
        for (const auto& i : common)
        {
            const double diff = fabs(value - i.first / static_cast<double>(i.second));
            if (diff < tolerance)
            {
                return i;
            }
        }
        return std::make_pair(static_cast<int>(value), 1);
    }
}
