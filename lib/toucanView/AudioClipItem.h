// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/IItem.h>
#include <toucanView/ItemLabel.h>
#include <toucanView/MarkerItem.h>

#include <dtk/ui/RowLayout.h>

#include <opentimelineio/clip.h>

namespace toucan
{
    //! Timeline audio clip item.
    class AudioClipItem : public IItem
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const ItemData&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&,
            const dtk::Color4F&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~AudioClipItem();

        //! Create a new item.
        static std::shared_ptr<AudioClipItem> create(
            const std::shared_ptr<dtk::Context>&,
            const ItemData&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&,
            const dtk::Color4F&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setScale(double) override;

        void setGeometry(const dtk::Box2I&) override;
        dtk::Box2I getChildrenClipRect() const override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;

    protected:
        void _timeUnitsUpdate() override;
        void _buildMenu(const std::shared_ptr<dtk::Menu>&) override;

    private:
        void _textUpdate();

        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip> _clip;
        std::string _text;
        dtk::Color4F _color;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<ItemLabel> _label;
        std::shared_ptr<TimeLayout> _markerLayout;
        std::vector<std::shared_ptr<MarkerItem> > _markerItems;

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int border = 0;
        };
        SizeData _size;

        struct GeomData
        {
            dtk::Box2I g2;
            dtk::Box2I g3;
        };
        GeomData _geom;
    };
}