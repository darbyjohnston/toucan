// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "FilesModel.h"

#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>

#include <filesystem>

namespace toucan
{
    class App;

    class StatusBar : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~StatusBar();

        static std::shared_ptr<StatusBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::shared_ptr<dtk::Label> _messageLabel;
        std::shared_ptr<dtk::Label> _infoLabel;

        std::shared_ptr<dtk::ValueObserver<File> > _fileObserver;
    };
}

