// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/ViewModel.h>

#include <dtk/ui/IWidget.h>

namespace toucan
{
    class App;
    class File;
    class HUDWidget;
    class Viewport;

    //! Timeline file tab.
    class FileTab : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<File>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~FileTab();

        //! Create a new tab.
        static std::shared_ptr<FileTab> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<File>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<Viewport> _viewport;
        std::shared_ptr<HUDWidget> _hudWidget;
        std::shared_ptr<dtk::ValueObserver<GlobalViewOptions> > _viewOptionsObserver;
    };
}

