// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Type.h"

#include <dtkUI/MenuBar.h>

namespace toucan
{
    class App;

    class MenuBar : public dtk::ui::MenuBar
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~MenuBar();

        static std::shared_ptr<MenuBar> create(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
        void _fileMenuInit(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&);
        void _frameMenuInit(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&);
        void _playbackMenuInit(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&);

        void _fileOpenAction();
        void _fileCloseAction();
        void _exitAction();
        void _playbackAction(Playback);

        std::weak_ptr<App> _app;
        std::map<std::string, std::shared_ptr<dtk::ui::Menu> > _menus;
        std::map<std::string, std::shared_ptr<dtk::ui::Action> > _actions;
        std::shared_ptr<dtk::core::ValueObserver<Playback> > _playbackObserver;
    };
}

