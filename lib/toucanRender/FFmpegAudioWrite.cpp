// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FFmpegAudioWrite.h"

#include <iostream>
#include <sstream>

extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
}

namespace toucan
{
    namespace ffmpeg
    {
        AudioWrite::AudioWrite(
            const std::filesystem::path& path,
            int sampleRate,
            int channelCount,
            AudioCodec audioCodec) :
            _path(path),
            _sampleRate(sampleRate),
            _channelCount(channelCount)
        {
            av_log_set_level(AV_LOG_QUIET);

            AVCodecID avCodecID = getAudioCodecId(audioCodec);

            int r = avformat_alloc_output_context2(&_avFormatContext, NULL, NULL, _path.string().c_str());
            if (r < 0)
            {
                throw std::runtime_error(getErrorLabel(r));
            }
            const AVCodec* avCodec = avcodec_find_encoder(avCodecID);
            if (!avCodec)
            {
                throw std::runtime_error("Cannot find audio encoder");
            }
            _avCodecContext = avcodec_alloc_context3(avCodec);
            if (!_avCodecContext)
            {
                throw std::runtime_error("Cannot allocate context");
            }
            _avAudioStream = avformat_new_stream(_avFormatContext, avCodec);
            if (!_avAudioStream)
            {
                throw std::runtime_error("Cannot allocate stream");
            }

            _avCodecContext->codec_id = avCodec->id;
            _avCodecContext->codec_type = AVMEDIA_TYPE_AUDIO;
            _avCodecContext->sample_rate = sampleRate;
            av_channel_layout_default(&_avCodecContext->ch_layout, channelCount);
            _avCodecContext->sample_fmt = avCodec->sample_fmts ?
                avCodec->sample_fmts[0] : getAudioSampleFormat(audioCodec);
            _avCodecContext->time_base = { 1, sampleRate };
            if (_avFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
            {
                _avCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }

            r = avcodec_open2(_avCodecContext, avCodec, NULL);
            if (r < 0)
            {
                throw std::runtime_error(getErrorLabel(r));
            }

            r = avcodec_parameters_from_context(_avAudioStream->codecpar, _avCodecContext);
            if (r < 0)
            {
                throw std::runtime_error(getErrorLabel(r));
            }

            _avAudioStream->time_base = { 1, sampleRate };

            r = avio_open(&_avFormatContext->pb, _path.string().c_str(), AVIO_FLAG_WRITE);
            if (r < 0)
            {
                throw std::runtime_error(getErrorLabel(r));
            }

            r = avformat_write_header(_avFormatContext, NULL);
            if (r < 0)
            {
                throw std::runtime_error(getErrorLabel(r));
            }

            _avPacket = av_packet_alloc();
            if (!_avPacket)
            {
                throw std::runtime_error("Cannot allocate packet");
            }

            _frameSize = _avCodecContext->frame_size;
            if (_frameSize <= 0)
            {
                _frameSize = 1024;
            }

            _avFrame = av_frame_alloc();
            if (!_avFrame)
            {
                throw std::runtime_error("Cannot allocate frame");
            }
            _avFrame->format = _avCodecContext->sample_fmt;
            _avFrame->ch_layout = _avCodecContext->ch_layout;
            _avFrame->sample_rate = sampleRate;
            _avFrame->nb_samples = _frameSize;
            r = av_frame_get_buffer(_avFrame, 0);
            if (r < 0)
            {
                throw std::runtime_error(getErrorLabel(r));
            }

            if (_avCodecContext->sample_fmt != AV_SAMPLE_FMT_FLT)
            {
                AVChannelLayout inLayout;
                av_channel_layout_default(&inLayout, channelCount);

                r = swr_alloc_set_opts2(
                    &_swrContext,
                    &_avCodecContext->ch_layout,
                    _avCodecContext->sample_fmt,
                    sampleRate,
                    &inLayout,
                    AV_SAMPLE_FMT_FLT,
                    sampleRate,
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
            }

            _opened = true;
        }

        AudioWrite::~AudioWrite()
        {
            if (_opened)
            {
                _flushFifo();
                _encodeAudio(nullptr);
                av_write_trailer(_avFormatContext);
            }
            if (_swrContext)
            {
                swr_free(&_swrContext);
            }
            if (_avFrame)
            {
                av_frame_free(&_avFrame);
            }
            if (_avPacket)
            {
                av_packet_free(&_avPacket);
            }
            if (_avCodecContext)
            {
                avcodec_free_context(&_avCodecContext);
            }
            if (_avFormatContext && _avFormatContext->pb)
            {
                avio_closep(&_avFormatContext->pb);
            }
            if (_avFormatContext)
            {
                avformat_free_context(_avFormatContext);
            }
        }

        void AudioWrite::writeAudio(const AudioBuffer& buffer)
        {
            _fifo.insert(_fifo.end(), buffer.data.begin(), buffer.data.end());

            while (static_cast<int>(_fifo.size()) / _channelCount >= _frameSize)
            {
                av_frame_make_writable(_avFrame);
                _avFrame->nb_samples = _frameSize;

                if (_swrContext)
                {
                    const uint8_t* inBuf = reinterpret_cast<const uint8_t*>(_fifo.data());
                    swr_convert(
                        _swrContext,
                        _avFrame->extended_data,
                        _frameSize,
                        &inBuf,
                        _frameSize);
                }
                else
                {
                    memcpy(
                        _avFrame->data[0],
                        _fifo.data(),
                        _frameSize * _channelCount * sizeof(float));
                }

                _fifo.erase(_fifo.begin(), _fifo.begin() + _frameSize * _channelCount);

                _avFrame->pts = _pts;
                _pts += _frameSize;
                _encodeAudio(_avFrame);
            }
        }

        void AudioWrite::_flushFifo()
        {
            if (_fifo.empty()) return;

            const int remainingSamples = static_cast<int>(_fifo.size()) / _channelCount;
            if (remainingSamples <= 0) return;

            av_frame_make_writable(_avFrame);
            _avFrame->nb_samples = remainingSamples;

            if (_swrContext)
            {
                const uint8_t* inBuf = reinterpret_cast<const uint8_t*>(_fifo.data());
                swr_convert(
                    _swrContext,
                    _avFrame->extended_data,
                    remainingSamples,
                    &inBuf,
                    remainingSamples);
            }
            else
            {
                memcpy(
                    _avFrame->data[0],
                    _fifo.data(),
                    remainingSamples * _channelCount * sizeof(float));
            }

            _fifo.clear();

            _avFrame->pts = _pts;
            _pts += remainingSamples;
            _encodeAudio(_avFrame);
        }

        void AudioWrite::_encodeAudio(AVFrame* frame)
        {
            int r = avcodec_send_frame(_avCodecContext, frame);
            if (r < 0)
            {
                throw std::runtime_error(getErrorLabel(r));
            }

            while (r >= 0)
            {
                r = avcodec_receive_packet(_avCodecContext, _avPacket);
                if (r == AVERROR(EAGAIN) || r == AVERROR_EOF)
                {
                    return;
                }
                else if (r < 0)
                {
                    throw std::runtime_error(getErrorLabel(r));
                }
                _avPacket->stream_index = _avAudioStream->index;
                r = av_interleaved_write_frame(_avFormatContext, _avPacket);
                if (r < 0)
                {
                    throw std::runtime_error(getErrorLabel(r));
                }
                av_packet_unref(_avPacket);
            }
        }
    }
}
