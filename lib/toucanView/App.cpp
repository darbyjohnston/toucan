// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "App.h"

#include "DocumentsModel.h"
#include "MainWindow.h"
#include "TimeUnitsModel.h"
#include "WindowModel.h"

#include <dtk/ui/DialogSystem.h>
#include <dtk/ui/FileBrowser.h>
#include <dtk/ui/MessageDialog.h>
#include <dtk/ui/RecentFilesModel.h>

#include <dtk/core/CmdLine.h>

#include <nlohmann/json.hpp>

namespace toucan
{
    void App::_init(
        const std::shared_ptr<dtk::Context>& context,
        std::vector<std::string>& argv)
    {
        _messageLog = std::make_shared<MessageLog>();

        _settings = dtk::Settings::create(dtk::getSettingsPath("toucan", "toucan-view.settings"));

        dtk::App::_init(
            context,
            argv,
            "toucan-view",
            "Toucan viewer",
            {
                dtk::CmdLineValueArg<std::string>::create(
                    _path,
                    "input",
                    "Input timeline.",
                    true)
            },
            {},
            _settings);
        
        _timeUnitsModel = std::make_shared<TimeUnitsModel>(_settings);

        std::vector<std::filesystem::path> searchPath;
        const std::filesystem::path parentPath = std::filesystem::path(argv[0]).parent_path();
        searchPath.push_back(parentPath);
#if defined(_WINDOWS)
        searchPath.push_back(parentPath / ".." / ".." / "..");
#else // _WINDOWS
        searchPath.push_back(parentPath / ".." / "..");
#endif // _WINDOWS
        ImageEffectHostOptions hostOptions;
        //hostOptions.log = _messageLog;
        _host = std::make_shared<ImageEffectHost>(searchPath, hostOptions);

        auto fileBrowserSystem = context->getSystem<dtk::FileBrowserSystem>();
        fileBrowserSystem->setNativeFileDialog(false);

        _documentsModel = std::make_shared<DocumentsModel>(context, _settings, _host);

        _windowModel = std::make_shared<WindowModel>();

        _window = MainWindow::create(
            context,
            std::dynamic_pointer_cast<App>(shared_from_this()),
            "toucan-view",
            dtk::Size2I(1920, 1080));
        addWindow(_window);

        if (!_path.empty())
        {
            try
            {
                _documentsModel->open(_path);
            }
            catch (const std::exception& e)
            {
                _context->getSystem<dtk::DialogSystem>()->message("ERROR", e.what(), _window);
            }
        }

        _window->show();
    }

    App::~App()
    {}

    std::shared_ptr<App> App::create(
        const std::shared_ptr<dtk::Context>& context,
        std::vector<std::string>& argv)
    {
        auto out = std::shared_ptr<App>(new App);
        out->_init(context, argv);
        return out;
    }

    const std::shared_ptr<TimeUnitsModel>& App::getTimeUnitsModel() const
    {
        return _timeUnitsModel;
    }

    const std::shared_ptr<ImageEffectHost>& App::getHost() const
    {
        return _host;
    }

    const std::shared_ptr<DocumentsModel>& App::getDocumentsModel() const
    {
        return _documentsModel;
    }

    const std::shared_ptr<WindowModel>& App::getWindowModel() const
    {
        return _windowModel;
    }
}
