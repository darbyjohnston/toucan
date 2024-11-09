// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "Timeline.h"

#include "Clip.h"
#include "Effects.h"
#include "Gap.h"
#include "MediaReferences.h"
#include "Stack.h"
#include "Track.h"
#include "Transitions.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/externalReference.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/generatorReference.h>
#include <opentimelineio/imageSequenceReference.h>
#include <opentimelineio/linearTimeWarp.h>
#include <opentimelineio/transition.h>

namespace toucan
{
    OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> convertTo(
        const std::shared_ptr<Timeline>& timeline)
    {
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> out;
        return out;
    }

    namespace
    {
        void convertEffects(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& otioItem,
            const std::shared_ptr<IItem>& item)
        {
            std::vector<std::shared_ptr<Effect> > effects;
            for (const auto& otioEffect : otioItem->effects())
            {
                if (auto otioLinear= OTIO_NS::dynamic_retainer_cast<OTIO_NS::LinearTimeWarp>(otioEffect))
                {
                    auto linear = std::make_shared<LinearTimeWarp>(
                        otioLinear->name(),
                        otioLinear->effect_name(),
                        otioLinear->metadata());
                    linear->setTimeScalar(otioLinear->time_scalar());
                    effects.push_back(linear);
                }
                else
                {
                    effects.push_back(std::make_shared<Effect>(
                        otioEffect->name(),
                        otioEffect->effect_name(),
                        otioEffect->metadata()));
                }
            }
            item->setEffects(effects);
        }

        std::shared_ptr<Clip> convertFrom(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& otioClip)
        {
            auto clip = std::make_shared<Clip>(
                otioClip->name(),
                otioClip->trimmed_range(),
                otioClip->metadata());

            Clip::MediaReferences refs;
            for (const auto& otioRef : otioClip->media_references())
            {
                if (auto otioExternalRef = dynamic_cast<OTIO_NS::ExternalReference*>(otioRef.second))
                {
                    refs[otioRef.first] = std::make_shared<ExternalReference>(
                        otioExternalRef->target_url());
                }
                else if (auto otioImageSequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(otioRef.second))
                {
                    refs[otioRef.first] = std::make_shared<ImageSequenceReference>(
                        otioImageSequenceRef->target_url_base(),
                        otioImageSequenceRef->name_prefix(),
                        otioImageSequenceRef->name_suffix(),
                        otioImageSequenceRef->start_frame(),
                        otioImageSequenceRef->frame_step(),
                        otioImageSequenceRef->rate(),
                        otioImageSequenceRef->frame_zero_padding());
                }
                else if (auto otioGeneratorRef = dynamic_cast<OTIO_NS::GeneratorReference*>(otioRef.second))
                {
                    refs[otioRef.first] = std::make_shared<GeneratorReference>(
                        otioGeneratorRef->generator_kind(),
                        otioGeneratorRef->parameters());
                }
            }
            clip->setMediaReferences(refs);
            clip->setActiveMediaReference(otioClip->active_media_reference_key());

            convertEffects(OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(otioClip), clip);

            return clip;
        }

        std::shared_ptr<Gap> convertFrom(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap>& otioGap)
        {
            auto out = std::make_shared<Gap>(
                otioGap->name(),
                otioGap->trimmed_range(),
                otioGap->metadata());
            return out;
        }

        std::shared_ptr<Transition> convertFrom(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Transition>& otioTransition)
        {
            auto out = std::make_shared<Transition>(
                otioTransition->name(),
                OTIO_NS::TimeRange(
                    OTIO_NS::RationalTime(0.0, otioTransition->duration().rate()),
                    OTIO_NS::RationalTime(0.0, otioTransition->duration().rate())),
                otioTransition->metadata(),
                otioTransition->transition_type(),
                otioTransition->in_offset(),
                otioTransition->out_offset());
            return out;
        }

        std::shared_ptr<Track> convertFrom(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& otioTrack)
        {
            auto track = std::make_shared<Track>(
                otioTrack->name(),
                otioTrack->trimmed_range(),
                otioTrack->metadata(),
                otioTrack->kind());
            
            for (const auto& otioItem : otioTrack->children())
            {
                if (auto otioClip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(otioItem))
                {
                    auto clip = convertFrom(otioClip);
                    track->addChild(clip);
                }
                else if (auto otioGap = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Gap>(otioItem))
                {
                    auto gap = convertFrom(otioGap);
                    track->addChild(gap);
                }
                else if (auto otioTransition = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Transition>(otioItem))
                {
                    auto transition = convertFrom(otioTransition);
                    track->addChild(transition);
                }
            }

            convertEffects(OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(otioTrack), track);

            return track;
        }
    }

    std::shared_ptr<Timeline> convertFrom(
        const std::filesystem::path& path,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& otioTimeline)
    {
        auto out = std::make_shared<Timeline>(path, otioTimeline->metadata());

        const OTIO_NS::RationalTime& duration = otioTimeline->duration();
        OTIO_NS::RationalTime startTime(0.0, duration.rate());
        auto opt = otioTimeline->global_start_time();
        if (opt.has_value())
        {
            startTime = opt.value();
        }
        out->setRange(OTIO_NS::TimeRange(startTime, duration));

        auto stack = out->getStack();
        auto otioStack = otioTimeline->tracks();
        stack->setName(otioStack->name());
        stack->setRange(otioStack->trimmed_range());
        stack->setMetadata(otioStack->metadata());

        for (const auto& otioItem : otioStack->children())
        {
            if (auto otioTrack = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(otioItem))
            {
                auto track = convertFrom(otioTrack);
                out->getStack()->addChild(track);
            }
        }

        convertEffects(otioStack, stack);

        return out;
    }
}
