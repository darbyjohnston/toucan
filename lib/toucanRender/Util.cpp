// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "Util.h"

#include <cctype>
#include <sstream>

namespace toucan
{
    std::string toLower(const std::string& value)
    {
        std::string out;
        for (auto i : value)
        {
            out.push_back(std::tolower(i));
        }
        return out;
    }

    std::pair<std::string, std::string> splitURLProtocol(const std::string& url)
    {
        std::pair<std::string, std::string> out;
        const size_t size = url.size();
        size_t pos = url.find("://");
        if (pos != std::string::npos)
        {
            out.first = url.substr(0, pos + 2);
            out.second = url.substr(pos + 3);
        }
        else
        {
            out.second = url;
        }
        return out;
    }

    std::string getSequenceFrame(
        const std::filesystem::path& path,
        const std::string& namePrefix,
        int frame,
        int padding,
        const std::string& nameSuffix)
    {
        std::stringstream ss;
        ss << namePrefix <<
            std::setw(padding) << std::setfill('0') << frame <<
            nameSuffix;
        return (path / ss.str()).string();
    }

    namespace
    {
        inline bool isNumber(char c)
        {
            return c >= '0' && c <= '9';
        }
    }

    std::pair<std::string, std::string> splitFileNameNumber(const std::string& stem)
    {
        std::pair<std::string, std::string> out;
        size_t i = stem.size();
        if (i > 0)
        {
            for (; i > 0 && isNumber(stem[i - 1]); --i)
                ;
        }
        if (i >= 0)
        {
            out.first = stem.substr(0, i);
            out.second = stem.substr(i, stem.size() - i);
        }
        return out;
    }

    size_t getNumberPadding(const std::string& value)
    {
        size_t out = 0;
        if (!value.empty() && '0' == value[0])
        {
            out = value.size();
        }
        return out;
    }

    IMATH_NAMESPACE::Box2i fit(
        const IMATH_NAMESPACE::V2i& a,
        const IMATH_NAMESPACE::V2i& b)
    {
        IMATH_NAMESPACE::Box2i out;
        int w = b.x;
        int h = b.y;
        const double aAspect = a.y > 0 ? (a.x / static_cast<double>(a.y)) : 1.0;
        const double bAspect = b.y > 0 ? (b.x / static_cast<double>(b.y)) : 1.0;
        if (bAspect > aAspect)
        {
            w = a.x;
            h = w / bAspect;
        }
        else
        {
            h = a.y;
            w = h * bAspect;
        }
        out.min.x = a.x / 2 - w / 2;
        out.min.y = a.y / 2 - h / 2;
        out.max.x = out.min.x + w - 1;
        out.max.y = out.min.y + h - 1;
        return out;
    }

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
}
