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
#include <libavutil/samplefmt.h>
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

        //! Audio codecs.
        enum class AudioCodec
        {
            PCM_S16LE,
            PCM_S24LE,
            PCM_F32LE,
            FLAC,
            AAC,

            Count,
            First = PCM_S16LE
        };

        //! Get a list of audio codecs.
        std::vector<AudioCodec> getAudioCodecs();

        //! Get a list of audio codec strings.
        std::vector<std::string> getAudioCodecStrings();

        //! Convert an audio codec to a string.
        std::string toString(AudioCodec);

        //! Convert a string to an audio codec.
        void fromString(const std::string&, AudioCodec&);

        //! Get an audio codec ID.
        AVCodecID getAudioCodecId(AudioCodec);

        //! Get an audio sample format.
        AVSampleFormat getAudioSampleFormat(AudioCodec);

        //! FFmpeg log callback.
        void log(void*, int level, const char* fmt, va_list vl);

        //! Get an error label.
        std::string getErrorLabel(int);
    }
}
