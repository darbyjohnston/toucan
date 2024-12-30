// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <string>
#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/log.h>
#include <libavutil/rational.h>
}

namespace toucan
{
    namespace ffmpeg
    {
        //! Swap the numerator and denominator.
        AVRational swap(AVRational);

        //! Video codecs.
        enum class VideoCodec
        {
            MJPEG,
            V210,
            V308,
            V408,
            V410,
            AV1,

            Count,
            First = MJPEG
        };

        //! Get a list of video codecs.
        std::vector<VideoCodec> getVideoCodecs();

        //! Get a list of video codec strings.
        std::vector<std::string> getVideoCodecStrings();

        //! Convert a video codec to a string.
        std::string toString(VideoCodec);

        //! Convert a string to a video codec.
        void fromString(const std::string&, VideoCodec&);

        //! Get a video codec ID.
        AVCodecID getVideoCodecId(VideoCodec);

        //! Get a video codec profile.
        int getVideoCodecProfile(VideoCodec);

        //! FFmpeg log callback.
        void log(void*, int level, const char* fmt, va_list vl);

        //! Get an error label.
        std::string getErrorLabel(int);
    }
}
