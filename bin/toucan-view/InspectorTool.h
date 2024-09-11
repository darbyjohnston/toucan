// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "IToolWidget.h"
#include "SelectionModel.h"

#include <dtk/ui/Bellows.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>

namespace toucan
{
    class Document;

    class InspectorWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~InspectorWidget();

        static std::shared_ptr<InspectorWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setOpen(bool);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> _item;
        std::shared_ptr<dtk::Bellows> _bellows;
    };

    class InspectorTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~InspectorTool();

        static std::shared_ptr<InspectorTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<dtk::VerticalLayout> _scrollLayout;
        std::vector<std::shared_ptr<InspectorWidget> > _widgets;

        std::shared_ptr<dtk::ListObserver<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > > _selectionObserver;
    };
}

