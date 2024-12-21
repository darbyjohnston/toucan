// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FileMenu.h"

#include "App.h"
#include "FilesModel.h"
#include "MainWindow.h"
#include "SelectionModel.h"

#include <dtk/ui/Action.h>
#include <dtk/ui/FileBrowser.h>
#include <dtk/ui/RecentFilesModel.h>
#include <dtk/core/Format.h>

namespace toucan
{
    void FileMenu::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& window,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::Menu::_init(context, parent);

        _filesModel = app->getFilesModel();

        auto appWeak = std::weak_ptr<App>(app);
        auto windowWeak = std::weak_ptr<MainWindow>(window);
        _actions["File/Open"] = std::make_shared<dtk::Action>(
            "Open",
            "FileOpen",
            dtk::Key::O,
            static_cast<int>(dtk::KeyModifier::Control),
            [this, appWeak, windowWeak]
            {
                auto context = getContext();
                auto fileBrowserSystem = context->getSystem<dtk::FileBrowserSystem>();
                if (_file)
                {
                    fileBrowserSystem->setPath(_file->getPath().parent_path());
                }
                dtk::FileBrowserOptions options = fileBrowserSystem->getOptions();
                options.extensions.clear();
                options.extensions.push_back(".otio");
                options.extensions.push_back(".otiod");
                options.extensions.push_back(".otioz");
                fileBrowserSystem->setOptions(options);
                fileBrowserSystem->open(
                    windowWeak.lock(),
                    [appWeak](const std::filesystem::path& path)
                    {
                        appWeak.lock()->open(path);
                    },
                    _filesModel->getRecentFilesModel());
            });
        _actions["File/Open"]->toolTip = "Open a file";
        addItem(_actions["File/Open"]);

        _actions["File/Close"] = std::make_shared<dtk::Action>(
            "Close",
            "FileClose",
            dtk::Key::E,
            static_cast<int>(dtk::KeyModifier::Control),
            [this] { _filesModel->close(); });
        _actions["File/Close"]->toolTip = "Close the current file";
        addItem(_actions["File/Close"]);

        _actions["File/CloseAll"] = std::make_shared<dtk::Action>(
            "Close All",
            "FileCloseAll",
            dtk::Key::E,
            static_cast<int>(dtk::KeyModifier::Shift) | static_cast<int>(dtk::KeyModifier::Control),
            [this] { _filesModel->closeAll(); });
        _actions["File/CloseAll"]->toolTip = "Close all files";
        addItem(_actions["File/CloseAll"]);

        addDivider();

        _menus["RecentFiles"] = addSubMenu("RecentFiles");

        addDivider();

        _menus["Files"] = addSubMenu("Files");

        _actions["File/Next"] = std::make_shared<dtk::Action>(
            "Next",
            dtk::Key::PageUp,
            0,
            [this] { _filesModel->next(); });
        _actions["File/Next"]->toolTip = "Switch to the next file";
        addItem(_actions["File/Next"]);

        _actions["File/Prev"] = std::make_shared<dtk::Action>(
            "Previous",
            dtk::Key::PageDown,
            0,
            [this] { _filesModel->prev(); });
        _actions["File/Prev"]->toolTip = "Switch to the previous file";
        addItem(_actions["File/Prev"]);

        addDivider();

        _actions["File/Exit"] = std::make_shared<dtk::Action>(
            "Exit",
            dtk::Key::Q,
            static_cast<int>(dtk::KeyModifier::Control),
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->exit();
                }
            });
        addItem(_actions["File/Exit"]);

        _filesObserver = dtk::ListObserver<std::shared_ptr<File> >::create(
            _filesModel->observeFiles(),
            [this](const std::vector<std::shared_ptr<File> >& files)
            {
                _menus["Files"]->clear();
                _filesActions.clear();
                for (int i = 0; i < files.size(); ++i)
                {
                    auto item = std::make_shared<dtk::Action>(
                        files[i]->getPath().filename().string(),
                        [this, i]
                        {
                            _filesModel->setCurrentIndex(i);
                            close();
                        });
                    _menus["Files"]->addItem(item);
                    _filesActions.push_back(item);
                }
            });

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            _filesModel->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                _menuUpdate();
            });

        _fileIndexObserver = dtk::ValueObserver<int>::create(
            _filesModel->observeCurrentIndex(),
            [this](int index)
            {
                for (int i = 0; i < _filesActions.size(); ++i)
                {
                    _menus["Files"]->setItemChecked(_filesActions[i], i == index);
                }
            });

        _recentFilesObserver = dtk::ListObserver<std::filesystem::path>::create(
            _filesModel->getRecentFilesModel()->observeRecent(),
            [this, appWeak](const std::vector<std::filesystem::path>& files)
            {
                _menus["RecentFiles"]->clear();
                _recentFilesActions.clear();
                for (auto i = files.rbegin(); i != files.rend(); ++i)
                {
                    auto file = *i;
                    auto item = std::make_shared<dtk::Action>(
                        file.string(),
                        [this, appWeak, file]
                        {
                            appWeak.lock()->open(file);
                            close();
                        });
                    _menus["RecentFiles"]->addItem(item);
                    _recentFilesActions.push_back(item);
                }
            });
    }

    FileMenu::~FileMenu()
    {}

    std::shared_ptr<FileMenu> FileMenu::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& window,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<FileMenu>(new FileMenu);
        out->_init(context, app, window, parent);
        return out;
    }

    const std::map<std::string, std::shared_ptr<dtk::Action> >& FileMenu::getActions() const
    {
        return _actions;
    }

    void FileMenu::_menuUpdate()
    {
        const bool file = _file.get();
        setItemEnabled(_actions["File/Close"], file);
        setItemEnabled(_actions["File/CloseAll"], file);
        setSubMenuEnabled(_menus["Files"], file);
        setItemEnabled(_actions["File/Next"], _filesActions.size() > 1);
        setItemEnabled(_actions["File/Prev"], _filesActions.size() > 1);
    }
}
