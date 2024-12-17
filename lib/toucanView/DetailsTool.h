// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/IToolWidget.h>
#include <toucanView/SelectionModel.h>
#include <toucanView/TimeUnitsModel.h>

#include <toucanRender/ImageGraph.h>

#include <dtk/ui/Bellows.h>
#include <dtk/ui/ButtonGroup.h>
#include <dtk/ui/GridLayout.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/ui/SearchBox.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/ObservableList.h>

#include <utility>

namespace toucan
{
    class File;

    //! Details widget.
    class DetailsWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<File>&,
            const SelectionItem&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~DetailsWidget();

        //! Create a new widget.
        static std::shared_ptr<DetailsWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<File>&,
            const SelectionItem&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        //! Set whether the widget is open.
        void setOpen(bool);

        //! Set the search.
        void setSearch(const std::string&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _textUpdate();
        void _searchUpdate();

        std::weak_ptr<App> _app;
        std::shared_ptr<File> _file;
        TimeUnits _timeUnits = TimeUnits::First;
        SelectionItem _item;
        std::vector<std::pair<std::string, std::string> > _text;
        std::string _search;

        std::shared_ptr<dtk::Bellows> _bellows;
        std::shared_ptr<dtk::GridLayout> _layout;
        std::shared_ptr<dtk::ToolButton> _startFrameButton;
        std::shared_ptr<dtk::ToolButton> _inOutButton;
        std::vector<std::pair<std::shared_ptr<dtk::Label>, std::shared_ptr<dtk::Label> > > _labels;

        std::shared_ptr<dtk::ValueObserver<TimeUnits> > _timeUnitsObserver;
    };

    //! Details tool.
    class DetailsTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~DetailsTool();

        //! Create a new tool.
        static std::shared_ptr<DetailsTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<dtk::VerticalLayout> _scrollLayout;
        std::vector<std::shared_ptr<DetailsWidget> > _widgets;
        std::shared_ptr<dtk::HorizontalLayout> _bottomLayout;
        std::shared_ptr<dtk::SearchBox> _searchBox;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
        std::shared_ptr<dtk::ListObserver<SelectionItem> > _selectionObserver;
    };
}

