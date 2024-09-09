// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "IItem.h"

#include <opentimelineio/track.h>

namespace toucan
{
    class TrackItem : public IItem
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TrackItem();

        static std::shared_ptr<TrackItem> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track> _track;
        dtk::Color4F _color = dtk::Color4F(.1F, .1F, .1F);

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int margin = 0;
        };
        SizeData _size;
    };
}
