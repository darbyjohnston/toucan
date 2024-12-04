// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <string>

extern "C"
{
#include <libavutil/log.h>
#include <libavutil/rational.h>
}

namespace toucan
{
    namespace ffmpeg
    {
        //! FFmpeg log callback.
        void log(void*, int level, const char* fmt, va_list vl);

        //! Swap the numerator and denominator.
        AVRational swap(AVRational);

        //! Check whether the given extension is compatible.
        bool isExtension(const std::string&);

        //! Get an error label.
        std::string getErrorLabel(int);
    }
}
