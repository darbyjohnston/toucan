// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FFmpegAudioRead.h"

#include <iostream>
#include <sstream>
#include <cmath>

extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
}

namespace toucan
{
    namespace ffmpeg
    {
        namespace
        {
            const size_t avIOContextBufferSize = 4096;

            class Packet
            {
            public:
                Packet()
                {
                    p = av_packet_alloc();
                }

                ~Packet()
                {
                    av_packet_free(&p);
                }

                AVPacket* p = nullptr;
            };
        }

        AudioRead::AudioRead(
            const std::filesystem::path& path,
            int outputSampleRate,
            int outputChannelCount,
            const MemoryReference& memoryReference) :
            _path(path),
            _memoryReference(memoryReference),
            _outputSampleRate(outputSampleRate),
            _outputChannelCount(outputChannelCount)
        {
            av_log_set_level(AV_LOG_QUIET);

            if (memoryReference.isValid())
            {
                _avFormatContext = avformat_alloc_context();
                if (!_avFormatContext)
                {
                    throw std::runtime_error("Cannot allocate format context");
                }

                _avIOBufferData = AVIOBufferData(
                    reinterpret_cast<const uint8_t*>(memoryReference.getData()),
                    memoryReference.getSize());
                _avIOContextBuffer = static_cast<uint8_t*>(av_malloc(avIOContextBufferSize));
                _avIOContext = avio_alloc_context(
                    _avIOContextBuffer,
                    avIOContextBufferSize,
                    0,
                    &_avIOBufferData,
                    &_avIOBufferRead,
                    nullptr,
                    &_avIOBufferSeek);
                if (!_avIOContext)
                {
                    throw std::runtime_error("Cannot allocate I/O context");
                }

                _avFormatContext->pb = _avIOContext;
            }

            const std::string fileName = path.string();
            int r = avformat_open_input(
                &_avFormatContext,
                !_avFormatContext ? fileName.c_str() : nullptr,
                nullptr,
                nullptr);
            if (r < 0 || !_avFormatContext)
            {
                throw std::runtime_error("Cannot open file");
            }

            r = avformat_find_stream_info(_avFormatContext, nullptr);
            if (r < 0)
            {
                throw std::runtime_error("Cannot find stream info");
            }

            for (unsigned int i = 0; i < _avFormatContext->nb_streams; ++i)
            {
                if (AVMEDIA_TYPE_AUDIO == _avFormatContext->streams[i]->codecpar->codec_type &&
                    AV_DISPOSITION_DEFAULT == _avFormatContext->streams[i]->disposition)
                {
                    _avStream = i;
                    break;
                }
            }
            if (-1 == _avStream)
            {
                for (unsigned int i = 0; i < _avFormatContext->nb_streams; ++i)
                {
                    if (AVMEDIA_TYPE_AUDIO == _avFormatContext->streams[i]->codecpar->codec_type)
                    {
                        _avStream = i;
                        break;
                    }
                }
            }

            if (_avStream != -1)
            {
                auto avAudioStream = _avFormatContext->streams[_avStream];
                auto avAudioCodecParameters = avAudioStream->codecpar;
                auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
                if (!avAudioCodec)
                {
                    throw std::runtime_error("No audio codec found");
                }
                _avCodecParameters = avcodec_parameters_alloc();
                if (!_avCodecParameters)
                {
                    throw std::runtime_error("Cannot allocate parameters");
                }
                avcodec_parameters_copy(_avCodecParameters, avAudioCodecParameters);
                _avCodecContext = avcodec_alloc_context3(avAudioCodec);
                if (!_avCodecContext)
                {
                    throw std::runtime_error("Cannot allocate context");
                }
                avcodec_parameters_to_context(_avCodecContext, _avCodecParameters);
                _avCodecContext->thread_count = 0;
                _avCodecContext->thread_type = FF_THREAD_FRAME;
                r = avcodec_open2(_avCodecContext, avAudioCodec, 0);
                if (r < 0)
                {
                    throw std::runtime_error("Cannot open audio stream");
                }

                _avFrame = av_frame_alloc();
                if (!_avFrame)
                {
                    throw std::runtime_error("Cannot allocate frame");
                }

                AVChannelLayout outLayout;
                av_channel_layout_default(&outLayout, _outputChannelCount);

                r = swr_alloc_set_opts2(
                    &_swrContext,
                    &outLayout,
                    AV_SAMPLE_FMT_FLT,
                    _outputSampleRate,
                    &_avCodecParameters->ch_layout,
                    static_cast<AVSampleFormat>(_avCodecParameters->format),
                    _avCodecParameters->sample_rate,
                    0,
                    nullptr);
                if (r < 0 || !_swrContext)
                {
                    throw std::runtime_error("Cannot allocate resampler context");
                }
                r = swr_init(_swrContext);
                if (r < 0)
                {
                    throw std::runtime_error("Cannot initialize resampler");
                }

                double duration = 0.0;
                if (avAudioStream->duration != AV_NOPTS_VALUE)
                {
                    duration = av_q2d(avAudioStream->time_base) * avAudioStream->duration;
                }
                else if (_avFormatContext->duration != AV_NOPTS_VALUE)
                {
                    duration = static_cast<double>(_avFormatContext->duration) / AV_TIME_BASE;
                }

                const double rate = _avCodecParameters->sample_rate;
                const int64_t totalSamples = static_cast<int64_t>(std::round(duration * rate));
                _timeRange = OTIO_NS::TimeRange(
                    OTIO_NS::RationalTime(0.0, rate),
                    OTIO_NS::RationalTime(totalSamples, rate));
                _currentTime = OTIO_NS::RationalTime(0.0, rate);
            }
        }

        AudioRead::~AudioRead()
        {
            if (_swrContext)
            {
                swr_free(&_swrContext);
            }
            if (_avFrame)
            {
                av_frame_free(&_avFrame);
            }
            if (_avCodecContext)
            {
                avcodec_free_context(&_avCodecContext);
            }
            if (_avCodecParameters)
            {
                avcodec_parameters_free(&_avCodecParameters);
            }
            if (_avIOContext)
            {
                avio_context_free(&_avIOContext);
            }
            if (_avFormatContext)
            {
                avformat_close_input(&_avFormatContext);
            }
        }

        bool AudioRead::hasAudio() const
        {
            return _avStream != -1;
        }

        int AudioRead::getSampleRate() const
        {
            return _outputSampleRate;
        }

        int AudioRead::getChannelCount() const
        {
            return _outputChannelCount;
        }

        const OTIO_NS::TimeRange& AudioRead::getTimeRange() const
        {
            return _timeRange;
        }

        AudioBuffer AudioRead::getAudio(
            const OTIO_NS::RationalTime& time,
            int sampleCount)
        {
            AudioBuffer out;
            out.sampleRate = _outputSampleRate;
            out.channelCount = _outputChannelCount;
            out.sampleCount = sampleCount;

            if (_avStream == -1)
            {
                out.data.resize(sampleCount * _outputChannelCount, 0.F);
                return out;
            }

            const OTIO_NS::RationalTime normalizedTime =
                time.rescaled_to(_timeRange.duration().rate());

            if (normalizedTime != _currentTime)
            {
                _seek(normalizedTime);
            }

            std::vector<float> samples;
            samples.reserve(sampleCount * _outputChannelCount);

            if (!_residual.empty())
            {
                const int residualSamples = static_cast<int>(
                    _residual.size()) / _outputChannelCount;
                if (residualSamples >= sampleCount)
                {
                    const int needed = sampleCount * _outputChannelCount;
                    samples.insert(samples.end(), _residual.begin(), _residual.begin() + needed);
                    _residual.erase(_residual.begin(), _residual.begin() + needed);
                    out.data = std::move(samples);
                    _currentTime = OTIO_NS::RationalTime(
                        _currentTime.value() +
                        static_cast<double>(sampleCount) *
                        _timeRange.duration().rate() / _outputSampleRate,
                        _timeRange.duration().rate());
                    return out;
                }
                samples.insert(samples.end(), _residual.begin(), _residual.end());
                _residual.clear();
            }

            const int remaining = sampleCount -
                static_cast<int>(samples.size()) / _outputChannelCount;
            _decode(samples, remaining);

            const int totalSamples = static_cast<int>(samples.size()) / _outputChannelCount;
            if (totalSamples > sampleCount)
            {
                const int needed = sampleCount * _outputChannelCount;
                _residual.assign(samples.begin() + needed, samples.end());
                samples.resize(needed);
            }
            else if (totalSamples < sampleCount)
            {
                samples.resize(sampleCount * _outputChannelCount, 0.F);
            }

            out.data = std::move(samples);
            _currentTime = OTIO_NS::RationalTime(
                _currentTime.value() +
                static_cast<double>(sampleCount) *
                _timeRange.duration().rate() / _outputSampleRate,
                _timeRange.duration().rate());
            return out;
        }

        void AudioRead::_seek(const OTIO_NS::RationalTime& time)
        {
            if (_avStream != -1)
            {
                avcodec_flush_buffers(_avCodecContext);
                swr_close(_swrContext);
                swr_init(_swrContext);

                const double seconds =
                    time.to_seconds() - _timeRange.start_time().to_seconds();
                const int64_t samplePos = static_cast<int64_t>(
                    seconds * _avCodecParameters->sample_rate);
                const int64_t timestamp = av_rescale_q(
                    samplePos,
                    { 1, _avCodecParameters->sample_rate },
                    _avFormatContext->streams[_avStream]->time_base);
                av_seek_frame(
                    _avFormatContext,
                    _avStream,
                    timestamp,
                    AVSEEK_FLAG_BACKWARD);
                _currentTime = time;
                _residual.clear();
            }
            _eof = false;
        }

        void AudioRead::_decode(std::vector<float>& output, int sampleCount)
        {
            if (_avStream == -1) return;

            const int initialSize = static_cast<int>(output.size());
            const int totalNeeded = initialSize + sampleCount * _outputChannelCount;

            Packet packet;
            int decoding = 0;
            while (0 == decoding && static_cast<int>(output.size()) < totalNeeded)
            {
                if (!_eof)
                {
                    decoding = av_read_frame(_avFormatContext, packet.p);
                    if (AVERROR_EOF == decoding)
                    {
                        _eof = true;
                        decoding = 0;
                    }
                    else if (decoding < 0)
                    {
                        break;
                    }
                }
                if ((_eof && _avStream != -1) || (_avStream == packet.p->stream_index))
                {
                    decoding = avcodec_send_packet(
                        _avCodecContext,
                        _eof ? nullptr : packet.p);
                    if (AVERROR_EOF == decoding)
                    {
                        decoding = 0;
                    }
                    else if (decoding < 0)
                    {
                        break;
                    }

                    while (0 == decoding)
                    {
                        decoding = avcodec_receive_frame(_avCodecContext, _avFrame);
                        if (decoding < 0)
                        {
                            break;
                        }

                        const int maxOutputSamples = swr_get_out_samples(
                            _swrContext, _avFrame->nb_samples);
                        if (maxOutputSamples <= 0) continue;

                        std::vector<float> converted(maxOutputSamples * _outputChannelCount);
                        uint8_t* outBuf = reinterpret_cast<uint8_t*>(converted.data());

                        const int convertedSamples = swr_convert(
                            _swrContext,
                            &outBuf,
                            maxOutputSamples,
                            const_cast<const uint8_t**>(_avFrame->extended_data),
                            _avFrame->nb_samples);

                        if (convertedSamples > 0)
                        {
                            output.insert(
                                output.end(),
                                converted.begin(),
                                converted.begin() + convertedSamples * _outputChannelCount);
                        }

                        if (static_cast<int>(output.size()) >= totalNeeded)
                        {
                            decoding = 1;
                            break;
                        }
                    }

                    if (AVERROR(EAGAIN) == decoding)
                    {
                        decoding = 0;
                    }
                    else if (AVERROR_EOF == decoding)
                    {
                        break;
                    }
                    else if (decoding < 0)
                    {
                        break;
                    }
                    else if (1 == decoding)
                    {
                        break;
                    }
                }
                if (packet.p->buf)
                {
                    av_packet_unref(packet.p);
                }
            }
            if (packet.p->buf)
            {
                av_packet_unref(packet.p);
            }
        }

        AudioRead::AVIOBufferData::AVIOBufferData()
        {
        }

        AudioRead::AVIOBufferData::AVIOBufferData(const uint8_t* data, size_t size) :
            data(data),
            size(size)
        {
        }

        int AudioRead::_avIOBufferRead(void* opaque, uint8_t* buf, int bufSize)
        {
            AVIOBufferData* bufferData = static_cast<AVIOBufferData*>(opaque);

            const int64_t remaining = bufferData->size - bufferData->offset;
            int bufSizeClamped = std::min(std::max(
                static_cast<int64_t>(bufSize),
                static_cast<int64_t>(0)),
                remaining);
            if (!bufSizeClamped)
            {
                return AVERROR_EOF;
            }

            memcpy(buf, bufferData->data + bufferData->offset, bufSizeClamped);
            bufferData->offset += bufSizeClamped;

            return bufSizeClamped;
        }

        int64_t AudioRead::_avIOBufferSeek(void* opaque, int64_t offset, int whence)
        {
            AVIOBufferData* bufferData = static_cast<AVIOBufferData*>(opaque);

            if (whence & AVSEEK_SIZE)
            {
                return bufferData->size;
            }

            bufferData->offset = std::min(std::max(
                offset,
                static_cast<int64_t>(0)),
                static_cast<int64_t>(bufferData->size));

            return offset;
        }
    }
}
