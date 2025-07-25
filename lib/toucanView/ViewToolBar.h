// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <feather-tk/ui/Action.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ToolButton.h>
#include <feather-tk/core/ObservableList.h>

namespace toucan
{
    class App;
    class File;

    //! View tool bar.
    class ViewToolBar : public feather_tk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<feather_tk::Context>&,
            const std::shared_ptr<App>&,
            const std::map<std::string, std::shared_ptr<feather_tk::Action> >&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ViewToolBar();

        //! Create a new tool bar.
        static std::shared_ptr<ViewToolBar> create(
            const std::shared_ptr<feather_tk::Context>&,
            const std::shared_ptr<App>&,
            const std::map<std::string, std::shared_ptr<feather_tk::Action> >&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const feather_tk::Box2I&) override;
        void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

    private:
        void _widgetUpdate();

        size_t _filesSize = 0;
        std::shared_ptr<File> _file;

        std::shared_ptr<feather_tk::HorizontalLayout> _layout;
        std::map<std::string, std::shared_ptr<feather_tk::ToolButton> > _buttons;

        std::shared_ptr<feather_tk::ListObserver<std::shared_ptr<File> > > _filesObserver;
        std::shared_ptr<feather_tk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
        std::shared_ptr<feather_tk::ValueObserver<bool> > _frameViewObserver;
    };
}

