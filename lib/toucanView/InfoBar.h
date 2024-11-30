// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/Action.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Label.h>

#include <dtk/core/ObservableList.h>

namespace toucan
{
    class App;
    class File;

    class InfoBar : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~InfoBar();

        static std::shared_ptr<InfoBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::shared_ptr<dtk::Label> _label;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
    };
}

