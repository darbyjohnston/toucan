// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FFmpeg.h"

#include "Util.h"

extern "C"
{
#include <libavutil/error.h>
}

#include <iostream>

namespace toucan
{
    namespace ffmpeg
    {
        void log(void*, int level, const char* fmt, va_list vl)
        {
            switch (level)
            {
            case AV_LOG_PANIC:
            case AV_LOG_FATAL:
            case AV_LOG_ERROR:
            case AV_LOG_WARNING:
            case AV_LOG_INFO:
            {
                char buf[4096];
                vsnprintf(buf, 4096, fmt, vl);
                std::cout << buf;
            }
            break;
            case AV_LOG_VERBOSE:
            default: break;
            }
        }

        AVRational swap(AVRational value)
        {
            return AVRational({ value.den, value.num });
        }

        bool isExtension(const std::string& extension)
        {
            const std::string lower = toLower(extension);
            return
                lower == ".mp4" ||
                lower == ".m4v" ||
                lower == ".mov";
        }

        std::string getErrorLabel(int r)
        {
            char buf[4096];
            av_strerror(r, buf, 4096);
            return std::string(buf);
        }
    }
}
