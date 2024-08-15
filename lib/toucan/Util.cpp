// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Util.h"

namespace toucan
{
    namespace
    {
        void _findPlugins(
            const std::filesystem::path& path,
            std::vector<std::filesystem::path>& out,
            int depth,
            int maxDepth)
        {
            for (auto const& entry : std::filesystem::directory_iterator(path))
            {
                const auto& entryPath = entry.path();
                if (entry.is_regular_file() &&
                    (entryPath.extension() == ".dll" || entryPath.extension() == ".so"))
                {
                    out.push_back(entryPath);
                }
                else if (entry.is_directory() && depth < maxDepth)
                {
                    _findPlugins(entryPath, out, depth + 1, maxDepth);
                }
            }
        }
    }

    void findPlugins(
        const std::filesystem::path& path,
        std::vector<std::filesystem::path>& out)
    {
        _findPlugins(path, out, 0, 2);
    }

    OTIO_NS::AnyVector vecToAny(const IMATH_NAMESPACE::V2i& vec)
    {
        return OTIO_NS::AnyVector{ vec.x, vec.y };
    }
    
    OTIO_NS::AnyVector vecToAny(const IMATH_NAMESPACE::V4f& vec)
    {
        return OTIO_NS::AnyVector{ vec.x, vec.y, vec.z, vec.w };
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
}
