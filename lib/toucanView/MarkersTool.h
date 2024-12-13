// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "IToolWidget.h"

#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/ui/SearchBox.h>
#include <dtk/core/ObservableList.h>

#include <opentimelineio/item.h>

namespace toucan
{
    class File;

    //! Markers tool.
    class MarkersTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~MarkersTool();

        //! Create a new tool.
        static std::shared_ptr<MarkersTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<File> _file;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::SearchBox> _searchBox;
        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<dtk::HorizontalLayout> _bottomLayout;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
        std::shared_ptr<dtk::ListObserver<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > > _selectionObserver;
    };
}

