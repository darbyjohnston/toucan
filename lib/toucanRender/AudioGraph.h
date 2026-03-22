// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/AudioBuffer.h>
#include <toucanRender/FFmpegAudioRead.h>
#include <toucanRender/TimelineWrapper.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/LRUCache.h>

#include <opentimelineio/track.h>
#include <opentimelineio/transition.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    class AudioGraph : public std::enable_shared_from_this<AudioGraph>
    {
    public:
        AudioGraph(
            const std::shared_ptr<ftk::Context>&,
            const std::filesystem::path&,
            const std::shared_ptr<TimelineWrapper>&,
            int sampleRate,
            int channelCount);

        ~AudioGraph();

        int getSampleRate() const;
        int getChannelCount() const;
        bool hasAudio() const;

        AudioBuffer exec(const OTIO_NS::RationalTime& time, int sampleCount);

    private:
        AudioBuffer _track(
            const OTIO_NS::RationalTime&,
            int sampleCount,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&);

        AudioBuffer _item(
            const OTIO_NS::RationalTime&,
            int sampleCount,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&);

        OTIO_NS::RationalTime _timeWarps(
            const OTIO_NS::RationalTime&,
            const OTIO_NS::TimeRange&,
            const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Effect> >&);

        void _applyEffects(
            AudioBuffer&,
            const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Effect> >&);

        std::weak_ptr<ftk::Context> _context;
        std::filesystem::path _path;
        std::shared_ptr<TimelineWrapper> _timelineWrapper;
        OTIO_NS::TimeRange _timeRange;
        int _sampleRate = 48000;
        int _channelCount = 2;
        bool _hasAudio = false;

        ftk::LRUCache<const OTIO_NS::MediaReference*, std::shared_ptr<ffmpeg::AudioRead> > _audioReadCache;
    };
}
