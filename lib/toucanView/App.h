// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/App.h>

namespace toucan
{
    class FilesModel;
    class ImageEffectHost;
    class MainWindow;
    class MessageLog;
    class TimeUnitsModel;
    class WindowModel;

    class App : public dtk::App
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            std::vector<std::string>&);

    public:
        virtual ~App();

        static std::shared_ptr<App> create(
            const std::shared_ptr<dtk::Context>&,
            std::vector<std::string>&);

        const std::shared_ptr<TimeUnitsModel>& getTimeUnitsModel() const;
        const std::shared_ptr<ImageEffectHost>& getHost() const;
        const std::shared_ptr<FilesModel>& getFilesModel() const;
        const std::shared_ptr<WindowModel>& getWindowModel() const;

    private:
        std::shared_ptr<MessageLog> _messageLog;
        std::string _path;
        std::shared_ptr<TimeUnitsModel> _timeUnitsModel;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<FilesModel> _filesModel;
        std::shared_ptr<WindowModel> _windowModel;
        std::shared_ptr<MainWindow> _window;
    };
}

