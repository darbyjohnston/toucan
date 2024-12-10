// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "File.h"

//#include <algorithm>
//#include <cctype>
//#include <iomanip>
#include <sstream>

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
            try
            {
                for (auto const& entry : std::filesystem::directory_iterator(path))
                {
                    const auto& entryPath = entry.path();
                    if (entry.is_regular_file() && entryPath.extension() == ".ofx")
                    {
                        out.push_back(entryPath);
                    }
                    else if (entry.is_directory() && depth < maxDepth)
                    {
                        _findPlugins(entryPath, out, depth + 1, maxDepth);
                    }
                }
            }
            catch (const std::exception&)
            {
                //! \bug How should this be handled?
            }
        }
    }

    void findPlugins(
        const std::filesystem::path& path,
        std::vector<std::filesystem::path>& out)
    {
        _findPlugins(path, out, 0, 2);
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
}
