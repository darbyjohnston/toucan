// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <opentimelineio/anyVector.h>

#include <OpenImageIO/imagebuf.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>

} // extern "C"

#include <filesystem>

namespace toucan
{
    class FFmpegRead : public std::enable_shared_from_this<FFmpegRead>
    {
    public:
        FFmpegRead(const std::filesystem::path&);

        virtual ~FFmpegRead();

        const OIIO::ImageSpec& getSpec();
        const OTIO_NS::TimeRange& getTimeRange() const;

        OIIO::ImageBuf getImage(const OTIO_NS::RationalTime&);

    private:
        void _seek(const OTIO_NS::RationalTime&);
        OIIO::ImageBuf _read();

        std::filesystem::path _path;
        OIIO::ImageSpec _spec;
        OTIO_NS::TimeRange _timeRange;
        OTIO_NS::RationalTime _currentTime;

        AVFormatContext* _avFormatContext = nullptr;
        uint8_t* _avIOContextBuffer = nullptr;
        AVIOContext* _avIOContext = nullptr;
        AVRational _avSpeed = { 24, 1 };
        int _avStream = -1;
        std::map<int, AVCodecParameters*> _avCodecParameters;
        std::map<int, AVCodecContext*> _avCodecContext;
        AVFrame* _avFrame = nullptr;
        AVFrame* _avFrame2 = nullptr;
        AVPixelFormat _avInputPixelFormat = AV_PIX_FMT_NONE;
        AVPixelFormat _avOutputPixelFormat = AV_PIX_FMT_NONE;
        SwsContext* _swsContext = nullptr;
        bool _eof = false;
    };
}
