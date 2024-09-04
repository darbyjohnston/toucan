// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/Action.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ToolButton.h>

namespace toucan
{
    class App;

    class ToolBar : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::map<std::string, std::shared_ptr<dtk::Action> >&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ToolBar();

        static std::shared_ptr<ToolBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::map<std::string, std::shared_ptr<dtk::Action> >&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::map<std::string, std::shared_ptr<dtk::ToolButton> > _buttons;
    };
}

