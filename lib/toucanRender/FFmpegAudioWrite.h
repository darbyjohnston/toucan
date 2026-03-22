// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/AudioBuffer.h>
#include <toucanRender/FFmpeg.h>

#include <opentimelineio/version.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

} // extern "C"

#include <filesystem>
#include <vector>

namespace toucan
{
    namespace ffmpeg
    {
        class AudioWrite : public std::enable_shared_from_this<AudioWrite>
        {
        public:
            AudioWrite(
                const std::filesystem::path&,
                int sampleRate,
                int channelCount,
                AudioCodec);

            virtual ~AudioWrite();

            void writeAudio(const AudioBuffer&);

        private:
            void _encodeAudio(AVFrame*);
            void _flushFifo();

            std::filesystem::path _path;
            int _sampleRate = 0;
            int _channelCount = 0;
            AVFormatContext* _avFormatContext = nullptr;
            AVCodecContext* _avCodecContext = nullptr;
            AVStream* _avAudioStream = nullptr;
            AVPacket* _avPacket = nullptr;
            AVFrame* _avFrame = nullptr;
            SwrContext* _swrContext = nullptr;
            int64_t _pts = 0;
            int _frameSize = 0;
            std::vector<float> _fifo;
            bool _opened = false;
        };
    }
}
