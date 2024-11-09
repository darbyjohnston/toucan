// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "StackTest.h"

#include <toucanEdit/Clip.h>
#include <toucanEdit/Stack.h>
#include <toucanEdit/Track.h>

#include <cassert>

namespace toucan
{
    void stackTest()
    {
        {
            auto stack = std::make_shared<Stack>();
            assert(stack == stack->getRoot());

            auto track = std::make_shared<Track>(TrackKind::video);
            stack->addChild(track);
            assert(stack == track->getRoot());

            auto clip = std::make_shared<Clip>();
            assert(!clip->getRoot());
            track->addChild(clip);
            assert(stack == clip->getRoot());
        }
        {
            auto track = std::make_shared<Track>(TrackKind::video);
            track->setRange(OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(0.0, 24.0),
                OTIO_NS::RationalTime(48.0, 24.0)));

            auto clip = std::make_shared<Clip>();
            clip->setRange(OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(0.0, 24.0),
                OTIO_NS::RationalTime(24.0, 24.0)));
            track->addChild(clip);

            auto clip2 = std::make_shared<Clip>();
            clip2->setRange(OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(0.0, 24.0),
                OTIO_NS::RationalTime(24.0, 24.0)));
            track->addChild(clip2);

            OTIO_NS::TimeRange range = track->getChildRange(clip);
            assert(OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(0.0, 24.0),
                OTIO_NS::RationalTime(24.0, 24.0)) == range);

            range = track->getChildRange(clip2);
            assert(OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(24.0, 24.0),
                OTIO_NS::RationalTime(24.0, 24.0)) == range);
        }
        {
            auto track = std::make_shared<Track>(TrackKind::video);
            track->setRange(OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(0.0, 24.0),
                OTIO_NS::RationalTime(48.0, 24.0)));

            auto clip = std::make_shared<Clip>();
            clip->setRange(OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(0.0, 24.0),
                OTIO_NS::RationalTime(24.0, 24.0)));
            track->addChild(clip);

            auto clip2 = std::make_shared<Clip>();
            clip2->setRange(OTIO_NS::TimeRange(
                OTIO_NS::RationalTime(0.0, 24.0),
                OTIO_NS::RationalTime(24.0, 24.0)));
            track->addChild(clip2);

            OTIO_NS::RationalTime t = track->transform(
                OTIO_NS::RationalTime(0.0, 24.0),
                clip);
            assert(OTIO_NS::RationalTime(0.0, 24.0) == t);

            t = track->transform(
                OTIO_NS::RationalTime(24.0, 24.0),
                clip2);
            assert(OTIO_NS::RationalTime(0.0, 24.0) == t);

            t = clip->transform(
                OTIO_NS::RationalTime(0.0, 24.0),
                track);
            assert(OTIO_NS::RationalTime(0.0, 24.0) == t);

            t = clip2->transform(
                OTIO_NS::RationalTime(0.0, 24.0),
                track);
            assert(OTIO_NS::RationalTime(24.0, 24.0) == t);
        }
    }
}
