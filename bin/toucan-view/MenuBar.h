// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "PlaybackModel.h"

#include <dtk/ui/MenuBar.h>

namespace toucan
{
    class App;

    class MenuBar : public dtk::MenuBar
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~MenuBar();

        static std::shared_ptr<MenuBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
        void _fileMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _frameMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _playbackMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);

        std::map<std::string, std::shared_ptr<dtk::Menu> > _menus;
        std::map<std::string, std::shared_ptr<dtk::Action> > _actions;
        std::shared_ptr<dtk::ValueObserver<Playback> > _playbackObserver;
    };
}

