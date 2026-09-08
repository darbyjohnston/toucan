// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/AudioBuffer.h>
#include <toucanRender/FFmpeg.h>

#include <opentimelineio/version.h>

#include <OpenImageIO/imagebuf.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>

} // extern "C"

#include <filesystem>
#include <vector>

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
                VideoCodec,
                int audioSampleRate = 0,
                int audioChannelCount = 0,
                AudioCodec audioCodec = AudioCodec::PCM_S16LE);

            virtual ~Write();

            void writeImage(const OIIO::ImageBuf&, const OTIO_NS::RationalTime&);
            void writeAudio(const AudioBuffer&);

        private:
            void _encodeVideo(AVFrame*);
            void _encodeAudio(AVFrame*);
            void _flushAudioFifo();

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

            AVCodecContext* _avAudioCodecContext = nullptr;
            AVStream* _avAudioStream = nullptr;
            AVPacket* _avAudioPacket = nullptr;
            AVFrame* _avAudioFrame = nullptr;
            SwrContext* _swrContext = nullptr;
            int64_t _audioPts = 0;
            int _audioSampleRate = 0;
            int _audioChannelCount = 0;
            int _audioFrameSize = 0;
            std::vector<float> _audioFifo;
        };
    }
}
