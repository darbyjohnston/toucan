// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/IItem.h>
#include <toucanView/ItemLabel.h>

#include <dtk/ui/RowLayout.h>

#include <opentimelineio/gap.h>

namespace toucan
{
    //! Timeline gap item.
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

        //! Create a new item.
        static std::shared_ptr<GapItem> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;

    protected:
        void _timeUnitsUpdate() override;

    private:
        void _textUpdate();

        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap> _gap;
        std::string _text;
        dtk::Color4F _color;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<ItemLabel> _label;

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int border = 0;
        };
        SizeData _size;
    };
}
