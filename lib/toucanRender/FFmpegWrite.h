// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/FFmpeg.h>

#include <opentimelineio/version.h>

#include <OpenImageIO/imagebuf.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

} // extern "C"

#include <filesystem>

namespace toucan
{
    namespace ffmpeg
    {
        class Write : public std::enable_shared_from_this<Write>
        {
        public:
            Write(
                const std::filesystem::path&,
                const OIIO::ImageSpec&,
                const OTIO_NS::TimeRange&,
                VideoCodec);

            virtual ~Write();

            void writeImage(const OIIO::ImageBuf&, const OTIO_NS::RationalTime&);

        private:
            void _encodeVideo(AVFrame*);

            std::filesystem::path _path;
            OIIO::ImageSpec _spec;
            OTIO_NS::TimeRange _timeRange;
            AVFormatContext* _avFormatContext = nullptr;
            AVCodecContext* _avCodecContext = nullptr;
            AVStream* _avVideoStream = nullptr;
            AVPacket* _avPacket = nullptr;
            AVFrame* _avFrame = nullptr;
            AVPixelFormat _avPixelFormatIn = AV_PIX_FMT_NONE;
            AVFrame* _avFrame2 = nullptr;
            SwsContext* _swsContext = nullptr;
            bool _opened = false;
        };
    }
}
