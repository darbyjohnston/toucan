// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "PlaybackModel.h"
#include "WindowModel.h"

#include <dtk/ui/MenuBar.h>
#include <dtk/core/ObservableList.h>
#include <dtk/core/ObservableMap.h>

#include <filesystem>

namespace toucan
{
    class App;
    class File;
    class FilesModel;
    class MainWindow;

    class MenuBar : public dtk::MenuBar
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~MenuBar();

        static std::shared_ptr<MenuBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        const std::map<std::string, std::shared_ptr<dtk::Action> >& getActions() const;

    private:
        void _fileMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _selectMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _timeMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _playbackMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _viewMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _windowMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&);

        void _fileMenuUpdate();
        void _selectMenuUpdate();
        void _timeMenuUpdate();
        void _playbackMenuUpdate();
        void _viewMenuUpdate();
        void _windowMenuUpdate();

        std::weak_ptr<App> _app;
        std::shared_ptr<FilesModel> _filesModel;
        std::shared_ptr<File> _file;

        std::map<std::string, std::shared_ptr<dtk::Menu> > _menus;
        std::map<std::string, std::shared_ptr<dtk::Action> > _actions;
        std::vector<std::shared_ptr<dtk::Action> > _filesActions;
        std::vector<std::shared_ptr<dtk::Action> > _recentFilesActions;

        std::shared_ptr<dtk::ListObserver<std::shared_ptr<File> > > _filesObserver;
        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _fileIndexObserver;
        std::shared_ptr<dtk::ListObserver<std::filesystem::path> > _recentFilesObserver;
        std::shared_ptr<dtk::ValueObserver<Playback> > _playbackObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _frameViewObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _fullScreenObserver;
        std::shared_ptr<dtk::MapObserver<WindowComponent, bool> > _componentsObserver;
        std::shared_ptr<dtk::ValueObserver<float> > _displayScaleObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _tooltipsObserver;
    };
}

