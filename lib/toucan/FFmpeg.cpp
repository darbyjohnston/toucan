// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FFmpeg.h"

#include "Util.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/error.h>
}

#include <algorithm>
#include <iostream>

namespace toucan
{
    namespace ffmpeg
    {
        AVRational swap(AVRational value)
        {
            return AVRational({ value.den, value.num });
        }

        std::vector<std::string> getVideoExtensions()
        {
            return std::vector<std::string>({ ".mov", ".mp4", ".m4v" });
        }

        bool hasVideoExtension(const std::string& value)
        {
            const std::vector<std::string> extensions = getVideoExtensions();
            const auto i = std::find(extensions.begin(), extensions.end(), value);
            return i != extensions.end();
        }

        namespace
        {
            std::vector<std::pair<int, std::string> > _getVideoCodecs()
            {
                std::vector<std::pair<int, std::string> > out;
                const AVCodec* avCodec = nullptr;
                void* avCodecIterate = nullptr;
                std::vector<std::string> codecNames;
                while ((avCodec = av_codec_iterate(&avCodecIterate)))
                {
                    if (av_codec_is_encoder(avCodec) &&
                        AVMEDIA_TYPE_VIDEO == avcodec_get_type(avCodec->id))
                    {
                        out.push_back({ avCodec->id, avCodec->name });
                    }
                }
                return out;
            }

            const std::vector<std::string> videoCodecStrings =
            {
                "MJPEG",
                "AV1"
            };

            const std::vector<AVCodecID> videoCodecIds =
            {
                AV_CODEC_ID_MJPEG,
                AV_CODEC_ID_AV1
            };

            const std::vector<int> videoCodecProfiles =
            {
                AV_PROFILE_UNKNOWN,
                AV_PROFILE_AV1_MAIN
            };
        }

        std::vector<VideoCodec> getVideoCodecs()
        {
            std::vector<VideoCodec> out;
            for (const auto& i : _getVideoCodecs())
            {
                for (size_t j = 0; j < videoCodecIds.size(); ++j)
                {
                    if (i.first == videoCodecIds[j])
                    {
                        out.push_back(static_cast<VideoCodec>(j));
                    }
                }
            }
            return out;
        }

        std::vector<std::string> getVideoCodecStrings()
        {
            std::vector<std::string> out;
            for (const auto& i : getVideoCodecs())
            {
                out.push_back(toString(i));
            }
            return out;
        }

        std::string toString(VideoCodec value)
        {
            return videoCodecStrings[static_cast<size_t>(value)];
        }
        
        void fromString(const std::string& s, VideoCodec& value)
        {
            const auto i = std::find(videoCodecStrings.begin(), videoCodecStrings.end(), s);
            value = i != videoCodecStrings.end() ?
                static_cast<VideoCodec>(i - videoCodecStrings.begin()) :
                VideoCodec::First;
        }

        AVCodecID getVideoCodecId(VideoCodec value)
        {
            return videoCodecIds[static_cast<size_t>(value)];
        }

        int getVideoCodecProfile(VideoCodec value)
        {
            return videoCodecProfiles[static_cast<size_t>(value)];
        }

        std::string getErrorLabel(int r)
        {
            char buf[4096];
            av_strerror(r, buf, 4096);
            return std::string(buf);
        }

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
    }
}
