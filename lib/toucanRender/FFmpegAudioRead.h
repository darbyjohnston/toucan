// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/AudioBuffer.h>
#include <toucanRender/FFmpeg.h>
#include <toucanRender/MemoryMap.h>

#include <opentimelineio/version.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

} // extern "C"

#include <filesystem>

namespace toucan
{
    namespace ffmpeg
    {
        class AudioRead : public std::enable_shared_from_this<AudioRead>
        {
        public:
            AudioRead(
                const std::filesystem::path&,
                int outputSampleRate,
                int outputChannelCount,
                const MemoryReference& = {});

            virtual ~AudioRead();

            bool hasAudio() const;
            int getSampleRate() const;
            int getChannelCount() const;
            const OTIO_NS::TimeRange& getTimeRange() const;

            AudioBuffer getAudio(
                const OTIO_NS::RationalTime& time,
                int sampleCount);

        private:
            void _seek(const OTIO_NS::RationalTime&);
            void _decode(std::vector<float>& output, int sampleCount);

            std::filesystem::path _path;
            MemoryReference _memoryReference;
            int _outputSampleRate = 0;
            int _outputChannelCount = 0;
            OTIO_NS::TimeRange _timeRange;
            OTIO_NS::RationalTime _currentTime;

            struct AVIOBufferData
            {
                AVIOBufferData();
                AVIOBufferData(const uint8_t*, size_t size);

                const uint8_t* data = nullptr;
                size_t size = 0;
                size_t offset = 0;
            };
            static int _avIOBufferRead(void* opaque, uint8_t* buf, int bufSize);
            static int64_t _avIOBufferSeek(void* opaque, int64_t offset, int whence);

            AVFormatContext* _avFormatContext = nullptr;
            AVIOBufferData _avIOBufferData;
            uint8_t* _avIOContextBuffer = nullptr;
            AVIOContext* _avIOContext = nullptr;
            int _avStream = -1;
            AVCodecParameters* _avCodecParameters = nullptr;
            AVCodecContext* _avCodecContext = nullptr;
            AVFrame* _avFrame = nullptr;
            SwrContext* _swrContext = nullptr;
            bool _eof = false;

            std::vector<float> _residual;
        };
    }
}
