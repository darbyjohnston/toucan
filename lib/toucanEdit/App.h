// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucan/ImageEffectHost.h>

#include <dtk/ui/App.h>

namespace toucan
{
    class DocumentsModel;
    class TimeUnitsModel;
    class Window;

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
        const std::shared_ptr<DocumentsModel>& getDocumentsModel() const;

    private:
        std::shared_ptr<MessageLog> _messageLog;
        std::string _path;
        std::shared_ptr<TimeUnitsModel> _timeUnitsModel;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<DocumentsModel> _documentsModel;
        std::shared_ptr<Window> _window;
    };
}

