// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "IToolWidget.h"

#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/ui/SearchBox.h>

namespace toucan
{
    class File;

    //! Message log widget.
    class MessageLogWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~MessageLogWidget();

        //! Create a new widget.
        static std::shared_ptr<MessageLogWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        //! Set the text.

        //! Set the filter.
        void setFilter(const std::string&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _textUpdate();

        std::vector<std::string> _text;
        std::string _filter;
        std::shared_ptr<dtk::Label> _label;
    };

    //! Message log tool.
    class MessageLogTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~MessageLogTool();

        //! Create a new tool.
        static std::shared_ptr<MessageLogTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<File> _file;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<dtk::VerticalLayout> _scrollLayout;
        std::shared_ptr<MessageLogWidget> _widget;
        std::shared_ptr<dtk::HorizontalLayout> _bottomLayout;
        std::shared_ptr<dtk::SearchBox> _searchBox;
    };
}

