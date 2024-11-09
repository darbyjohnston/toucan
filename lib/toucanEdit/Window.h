// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/RowLayout.h>
#include <dtk/ui/TabWidget.h>
#include <dtk/ui/Window.h>
#include <dtk/core/ObservableList.h>

namespace toucan
{
    class App;
    class Document;
    class DocumentTab;
    class MenuBar;

    class Window : public dtk::Window
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::string& name,
            const dtk::Size2I&);

    public:
        virtual ~Window();

        static std::shared_ptr<Window> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::string& name,
            const dtk::Size2I&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void keyPressEvent(dtk::KeyEvent&) override;
        void keyReleaseEvent(dtk::KeyEvent&) override;

    protected:
        void _drop(const std::vector<std::string>&) override;

    private:
        std::weak_ptr<App> _app;
        std::vector<std::shared_ptr<Document> > _documents;

        std::shared_ptr<dtk::RowLayout> _layout;
        std::shared_ptr<MenuBar> _menuBar;
        std::shared_ptr<dtk::TabWidget> _tabWidget;
        std::map<std::shared_ptr<Document>, std::shared_ptr<DocumentTab> > _documentTabs;

        std::shared_ptr<dtk::ListObserver<std::shared_ptr<Document> > > _documentsObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _addObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _removeObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _documentObserver;
    };
}

