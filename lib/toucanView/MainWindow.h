// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/WindowModel.h>

#include <dtk/ui/Divider.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Splitter.h>
#include <dtk/ui/TabWidget.h>
#include <dtk/ui/Window.h>
#include <dtk/core/ObservableList.h>

namespace toucan
{
    class App;
    class File;
    class FileTab;
    class IToolWidget;
    class InfoBar;
    class MenuBar;
    class PlaybackBar;
    class TimelineWidget;
    class ToolBar;

    //! Main window.
    class MainWindow : public dtk::Window
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::string& name,
            const dtk::Size2I&);

    public:
        virtual ~MainWindow();

        //! Create a new window.
        static std::shared_ptr<MainWindow> create(
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
        std::vector<std::shared_ptr<File> > _files;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<MenuBar> _menuBar;
        std::shared_ptr<ToolBar> _toolBar;
        std::shared_ptr<dtk::Divider> _toolBarDivider;
        std::shared_ptr<dtk::Splitter> _vSplitter;
        std::shared_ptr<dtk::Splitter> _hSplitter;
        std::shared_ptr<dtk::TabWidget> _tabWidget;
        std::map<std::shared_ptr<File>, std::shared_ptr<FileTab> > _fileTabs;
        std::shared_ptr<dtk::TabWidget> _toolWidget;
        std::vector<std::shared_ptr<IToolWidget> > _toolWidgets;
        std::shared_ptr<dtk::VerticalLayout> _bottomLayout;
        std::shared_ptr<PlaybackBar> _playbackBar;
        std::shared_ptr<TimelineWidget> _timelineWidget;
        std::shared_ptr<InfoBar> _infoBar;

        std::shared_ptr<dtk::ListObserver<std::shared_ptr<File> > > _filesObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _addObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _removeObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _fileObserver;
        std::shared_ptr<dtk::MapObserver<WindowComponent, bool> > _componentsObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _tooltipsObserver;
    };
}

