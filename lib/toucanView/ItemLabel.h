// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/IWidget.h>

namespace toucan
{
    //! Timeline item label.
    class ItemLabel : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ItemLabel();

        //! Create a new label.
        static std::shared_ptr<ItemLabel> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        //! Set the name.
        void setName(const std::string&);

        //! Set the duration.
        void setDuration(const std::string&);

        //! Set the margin size role.
        void setMarginRole(dtk::SizeRole);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void clipEvent(const dtk::Box2I&, bool) override;
        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;

    private:
        std::string _name;
        std::string _duration;
        dtk::SizeRole _marginRole = dtk::SizeRole::MarginInside;

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int margin = 0;
            int margin2 = 0;
            dtk::FontInfo fontInfo;
            dtk::FontMetrics fontMetrics;
            dtk::Size2I nameSize;
            dtk::Size2I durationSize;
        };
        SizeData _size;

        struct DrawData
        {
            std::vector<std::shared_ptr<dtk::Glyph> > nameGlyphs;
            std::vector<std::shared_ptr<dtk::Glyph> > durationGlyphs;
        };
        DrawData _draw;
    };
}

