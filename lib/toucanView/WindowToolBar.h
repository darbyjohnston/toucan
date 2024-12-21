// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/WindowModel.h>

#include <dtk/ui/Action.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/ObservableList.h>

namespace toucan
{
    class App;
    class File;
    class MainWindow;

    //! Window tool bar.
    class WindowToolBar : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&,
            const std::map<std::string, std::shared_ptr<dtk::Action> >&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~WindowToolBar();

        //! Create a new tool bar.
        static std::shared_ptr<WindowToolBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<MainWindow>&,
            const std::map<std::string, std::shared_ptr<dtk::Action> >&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::map<std::string, std::shared_ptr<dtk::ToolButton> > _buttons;

        std::shared_ptr<dtk::ValueObserver<bool> > _frameViewObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _fullScreenObserver;
        std::shared_ptr<dtk::MapObserver<WindowComponent, bool> > _componentObserver;
    };
}

