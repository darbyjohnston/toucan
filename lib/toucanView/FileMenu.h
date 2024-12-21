// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/Menu.h>
#include <dtk/core/ObservableList.h>
#include <dtk/core/ObservableValue.h>

#include <filesystem>
#include <map>

namespace toucan
{
    class App;
    class File;
    class FilesModel;
    class MainWindow;

    //! File bar.
    class FileMenu : public dtk::Menu
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~FileMenu();

        //! Create a new menu.
        static std::shared_ptr<FileMenu> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        //! Get the actions.
        const std::map<std::string, std::shared_ptr<dtk::Action> >& getActions() const;

    private:
        void _menuUpdate();

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
    };
}

