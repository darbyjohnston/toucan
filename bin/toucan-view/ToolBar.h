// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/Action.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ToolButton.h>

#include <dtk/core/ObservableList.h>

namespace toucan
{
    class App;
    class Document;
    class Window;

    class ToolBar : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Window>&,
            const std::map<std::string, std::shared_ptr<dtk::Action> >&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ToolBar();

        static std::shared_ptr<ToolBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Window>&,
            const std::map<std::string, std::shared_ptr<dtk::Action> >&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _widgetUpdate();

        size_t _documentsSize = 0;

        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::map<std::string, std::shared_ptr<dtk::ToolButton> > _buttons;

        std::shared_ptr<dtk::ListObserver<std::shared_ptr<Document> > > _documentsObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _fullScreenObserver;
    };
}

