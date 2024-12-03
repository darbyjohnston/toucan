// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "IItem.h"

#include <opentimelineio/clip.h>

namespace toucan
{
    //! Timeline clip item.
    class ClipItem : public IItem
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>&,
            const dtk::Color4F&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ClipItem();

        //! Create a new item.
        static std::shared_ptr<ClipItem> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>&,
            const dtk::Color4F&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void clipEvent(const dtk::Box2I&, bool) override;
        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;

    private:
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip> _clip;
        std::string _text;
        dtk::Color4F _color;

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int margin = 0;
            int border = 0;
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
