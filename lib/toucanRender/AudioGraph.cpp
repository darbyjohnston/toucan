// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "AudioGraph.h"

#include "TimelineAlgo.h"
#include "Util.h"

#include <ftk/Core/LogSystem.h>

#include <opentimelineio/clip.h>
#include <opentimelineio/externalReference.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/linearTimeWarp.h>

#include <algorithm>
#include <cmath>

namespace toucan
{
    namespace
    {
        const std::string logPrefix = "toucan::AudioGraph";
    }

    AudioGraph::AudioGraph(
        const std::shared_ptr<ftk::Context>& context,
        const std::filesystem::path& path,
        const std::shared_ptr<TimelineWrapper>& timelineWrapper,
        int sampleRate,
        int channelCount) :
        _context(context),
        _path(path),
        _timelineWrapper(timelineWrapper),
        _timeRange(timelineWrapper->getTimeRange()),
        _sampleRate(sampleRate),
        _channelCount(channelCount)
    {
        _audioReadCache.setMax(20);

        for (const auto& child : _timelineWrapper->getTimeline()->tracks()->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(child))
            {
                if (track->kind() == OTIO_NS::Track::Kind::audio &&
                    !track->find_clips().empty())
                {
                    _hasAudio = true;
                    break;
                }
            }
        }
    }

    AudioGraph::~AudioGraph()
    {}

    int AudioGraph::getSampleRate() const
    {
        return _sampleRate;
    }

    int AudioGraph::getChannelCount() const
    {
        return _channelCount;
    }

    bool AudioGraph::hasAudio() const
    {
        return _hasAudio;
    }

    AudioBuffer AudioGraph::exec(const OTIO_NS::RationalTime& time, int sampleCount)
    {
        AudioBuffer out;
        out.sampleRate = _sampleRate;
        out.channelCount = _channelCount;
        out.sampleCount = sampleCount;
        out.data.resize(sampleCount * _channelCount, 0.F);

        auto stack = _timelineWrapper->getTimeline()->tracks();
        const auto& stackEffects = stack->effects();
        OTIO_NS::RationalTime t = time - _timeRange.start_time();
        t = _timeWarps(t, stack->available_range(), stackEffects);

        int trackCount = 0;
        for (const auto& i : stack->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(i))
            {
                // Audio comes from the audio tracks. A video clip's media may
                // carry audio too, and it is left alone: the timeline says
                // what is heard, the way it says what is seen.
                if (track->kind() == OTIO_NS::Track::Kind::audio &&
                    !track->find_clips().empty())
                {
                    const auto& trackEffects = track->effects();
                    OTIO_NS::RationalTime t2 = t;
                    if (!trackEffects.empty())
                    {
                        t2 = _timeWarps(t2, track->available_range(), trackEffects);
                    }

                    AudioBuffer trackBuf = _track(t2, sampleCount, track);

                    _applyEffects(trackBuf, trackEffects);

                    if (trackBuf.isValid())
                    {
                        for (size_t j = 0; j < out.data.size() && j < trackBuf.data.size(); ++j)
                        {
                            out.data[j] += trackBuf.data[j];
                        }
                        ++trackCount;
                    }
                }
            }
        }

        for (size_t j = 0; j < out.data.size(); ++j)
        {
            out.data[j] = std::max(-1.F, std::min(1.F, out.data[j]));
        }

        return out;
    }

    AudioBuffer AudioGraph::_track(
        const OTIO_NS::RationalTime& time,
        int sampleCount,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track)
    {
        AudioBuffer out;
        out.sampleRate = _sampleRate;
        out.channelCount = _channelCount;
        out.sampleCount = sampleCount;
        out.data.resize(sampleCount * _channelCount, 0.F);

        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> item;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Composable> prev;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Composable> prev2;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Composable> next;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Composable> next2;
        const auto& children = track->children();
        for (size_t i = 0; i < children.size(); ++i)
        {
            if ((item = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(children[i])))
            {
                const auto trimmedRangeInParent = item->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    out = _item(
                        track->transformed_time(time, item),
                        sampleCount,
                        item);
                    if (i > 0)
                    {
                        prev = children[i - 1];
                    }
                    if (i > 1)
                    {
                        prev2 = children[i - 2];
                    }
                    if (i < (children.size() - 1))
                    {
                        next = children[i + 1];
                    }
                    if (children.size() > 1 && i < (children.size() - 2))
                    {
                        next2 = children[i + 2];
                    }
                    break;
                }
            }
        }

        if (item)
        {
            if (auto prevTransition = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Transition>(prev))
            {
                const auto trimmedRangeInParent = prevTransition->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    if (auto prevItem = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(prev2))
                    {
                        AudioBuffer a = _item(
                            track->transformed_time(time, prevItem),
                            sampleCount,
                            prevItem);
                        _crossfade(a, out, time, trimmedRangeInParent.value(), out);
                    }
                }
            }
            if (auto nextTransition = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Transition>(next))
            {
                const auto trimmedRangeInParent = nextTransition->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    if (auto nextItem = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(next2))
                    {
                        AudioBuffer b = _item(
                            track->transformed_time(time, nextItem),
                            sampleCount,
                            nextItem);
                        _crossfade(out, b, time, trimmedRangeInParent.value(), out);
                    }
                }
            }
        }

        return out;
    }

    void AudioGraph::_crossfade(
        const AudioBuffer& from,
        const AudioBuffer& to,
        const OTIO_NS::RationalTime& time,
        const OTIO_NS::TimeRange& range,
        AudioBuffer& out)
    {
        if (!from.isValid() || !to.isValid() || !out.isValid())
        {
            return;
        }
        // The mix moves across the frame rather than stepping at each
        // frame boundary: one gain at the frame's start, another at its end,
        // and a line between them for the samples.
        const double duration = range.duration().value();
        const double v0 = std::clamp(
            (time - range.start_time()).value() / duration, 0.0, 1.0);
        const double v1 = std::clamp(
            (time + OTIO_NS::RationalTime(1.0, time.rate()) - range.start_time()).value() / duration,
            0.0,
            1.0);
        const size_t count = std::min({ out.data.size(), from.data.size(), to.data.size() });
        const int channels = std::max(1, out.channelCount);
        const int samples = std::max<int>(1, out.sampleCount);
        for (size_t j = 0; j < count; ++j)
        {
            const double v = v0 + (v1 - v0) * (static_cast<double>(j / channels) / samples);
            out.data[j] = from.data[j] * static_cast<float>(1.0 - v) +
                to.data[j] * static_cast<float>(v);
        }
    }

    AudioBuffer AudioGraph::_item(
        const OTIO_NS::RationalTime& time,
        int sampleCount,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item)
    {
        AudioBuffer out;
        out.sampleRate = _sampleRate;
        out.channelCount = _channelCount;
        out.sampleCount = sampleCount;
        out.data.resize(sampleCount * _channelCount, 0.F);

        OTIO_NS::RationalTime t = time;

        const auto& effects = item->effects();
        t = _timeWarps(t, item->available_range(), effects);

        if (auto clip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(item))
        {
            auto mediaRef = clip->media_reference();
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(mediaRef))
            {
                std::shared_ptr<ffmpeg::AudioRead> audioRead;
                if (!_audioReadCache.get(externalRef, audioRead))
                {
                    try
                    {
                        const std::string mediaPath =
                            _timelineWrapper->getMediaPath(externalRef->target_url());
                        audioRead = std::make_shared<ffmpeg::AudioRead>(
                            mediaPath, _sampleRate, _channelCount);
                    }
                    catch (const std::exception& e)
                    {
                        _context.lock()->getSystem<ftk::LogSystem>()->print(
                            logPrefix,
                            e.what(),
                            ftk::LogType::Error);
                    }
                    // Cached either way: media that did not open is not
                    // opened, and logged, again on every frame.
                    _audioReadCache.add(externalRef, audioRead);
                }
                if (audioRead && audioRead->hasAudio())
                {
                    if (clip->available_range().start_time() !=
                        audioRead->getTimeRange().start_time())
                    {
                        t -= clip->available_range().start_time();
                    }

                    out = audioRead->getAudio(t, sampleCount);
                }
            }
        }
        else if (auto gap = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Gap>(item))
        {
        }

        _applyEffects(out, effects);

        return out;
    }

    OTIO_NS::RationalTime AudioGraph::_timeWarps(
        const OTIO_NS::RationalTime& time,
        const OTIO_NS::TimeRange& timeRange,
        const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Effect> >& effects)
    {
        OTIO_NS::RationalTime out = time;
        for (const auto& effect : effects)
        {
            if (auto linearTimeWarp = dynamic_cast<OTIO_NS::LinearTimeWarp*>(effect.value))
            {
                // Scaled about the range start, which is then put back, as
                // the image graph does.
                const double s = linearTimeWarp->time_scalar();
                const OTIO_NS::RationalTime start = timeRange.start_time().rescaled_to(time.rate());
                out = OTIO_NS::RationalTime(
                    start.value() + (out - start).value() * s,
                    time.rate()).round();
            }
        }
        return out;
    }

    void AudioGraph::_applyEffects(
        AudioBuffer& buffer,
        const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Effect> >& effects)
    {
        for (const auto& effect : effects)
        {
            if (dynamic_cast<OTIO_NS::LinearTimeWarp*>(effect.value))
            {
                continue;
            }

            const auto& metaData = effect->metadata();
            auto volumeIt = metaData.find("volume");
            if (volumeIt != metaData.end() && volumeIt->second.has_value())
            {
                try
                {
                    const float volume = std::any_cast<double>(volumeIt->second);
                    for (size_t i = 0; i < buffer.data.size(); ++i)
                    {
                        buffer.data[i] *= volume;
                    }
                }
                catch (const std::bad_any_cast&)
                {
                }
            }
        }
    }
}
