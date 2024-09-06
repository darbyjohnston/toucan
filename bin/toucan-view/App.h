// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "DocumentsModel.h"
#include "TimeUnitsModel.h"
#include "Window.h"

#include <dtk/ui/App.h>

namespace toucan
{
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

        const std::shared_ptr<DocumentsModel>& getDocumentsModel() const;

    private:
        std::shared_ptr<MessageLog> _messageLog;
        std::filesystem::path _path;
        std::shared_ptr<TimeUnitsModel> _timeUnitsModel;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<DocumentsModel> _documentsModel;
        std::shared_ptr<Window> _window;
    };
}

