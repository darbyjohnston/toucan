// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/FilesModel.h>

#include <feather-tk/ui/Menu.h>

#include <map>

namespace toucan
{
    class App;

    //! Compare menu.
    class CompareMenu : public feather_tk::Menu
    {
    protected:
        void _init(
            const std::shared_ptr<feather_tk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~CompareMenu();

        //! Create a new menu.
        static std::shared_ptr<CompareMenu> create(
            const std::shared_ptr<feather_tk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        //! Get the actions.
        const std::map<std::string, std::shared_ptr<feather_tk::Action> >& getActions() const;

    private:
        void _menuUpdate();

        std::shared_ptr<FilesModel> _filesModel;
        std::shared_ptr<File> _file;

        std::map<std::string, std::shared_ptr<feather_tk::Menu> > _menus;
        std::map<std::string, std::shared_ptr<feather_tk::Action> > _actions;
        std::vector<std::shared_ptr<feather_tk::Action> > _bFileActions;

        std::shared_ptr<feather_tk::ListObserver<std::shared_ptr<File> > > _filesObserver;
        std::shared_ptr<feather_tk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
        std::shared_ptr<feather_tk::ValueObserver<int> > _bIndexObserver;
        std::shared_ptr<feather_tk::ValueObserver<CompareOptions> > _modeObserver;
    };
}
