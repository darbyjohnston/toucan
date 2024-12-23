// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/IToolWidget.h>
#include <toucanView/SelectionModel.h>

#include <toucanRender/ImageGraph.h>

#include <dtk/ui/Bellows.h>
#include <dtk/ui/ButtonGroup.h>
#include <dtk/ui/GridLayout.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/ui/SearchBox.h>
#include <dtk/core/ObservableList.h>

#include <utility>

namespace toucan
{
    class File;

    //! Information item widget.
    class InfoItemWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~InfoItemWidget();

        //! Create a new widget.
        static std::shared_ptr<InfoItemWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        //! Set whether the widget is open.
        void setOpen(bool);

        //! Set the search.
        void setSearch(const std::string&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _textUpdate();

        OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata> _object;
        std::vector<std::pair<std::string, std::string> > _text;
        std::string _search;
        std::shared_ptr<dtk::Bellows> _bellows;
        std::shared_ptr<dtk::GridLayout> _layout;
        std::vector<std::pair<std::shared_ptr<dtk::Label>, std::shared_ptr<dtk::Label> > > _labels;
    };

    //! Infoformation tool.
    class InfoTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~InfoTool();

        //! Create a new tool.
        static std::shared_ptr<InfoTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<dtk::VerticalLayout> _scrollLayout;
        std::vector<std::shared_ptr<InfoItemWidget> > _widgets;
        std::shared_ptr<dtk::HorizontalLayout> _bottomLayout;
        std::shared_ptr<dtk::SearchBox> _searchBox;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
        std::shared_ptr<dtk::ListObserver<SelectionItem> > _selectionObserver;
    };
}

