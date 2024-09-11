// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "IItem.h"

#include <opentimelineio/gap.h>

namespace toucan
{
    class GapItem : public IItem
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~GapItem();

        static std::shared_ptr<GapItem> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void clipEvent(const dtk::Box2I&, bool) override;
        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;

    private:
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap> _gap;
        std::string _text;
        dtk::Color4F _color;

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int margin = 0;
            int border = 0;
            int borderFocus = 0;
            dtk::FontInfo fontInfo;
            dtk::FontMetrics fontMetrics;
            dtk::Size2I textSize;
        };
        SizeData _size;

        struct DrawData
        {
            std::vector<std::shared_ptr<dtk::Glyph> > glyphs;
        };
        DrawData _draw;
    };
}
