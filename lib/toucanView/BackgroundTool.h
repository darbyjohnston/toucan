// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/FilesModel.h>
#include <toucanView/ViewModel.h>
#include <toucanView/IToolWidget.h>

#include <dtk/ui/ColorWidget.h>
#include <dtk/ui/ComboBox.h>
#include <dtk/ui/IntEditSlider.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>

namespace toucan
{
    class File;

    //! Background tool.
    class BackgroundTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~BackgroundTool();

        //! Create a new tool.
        static std::shared_ptr<BackgroundTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _widgetUpdate();

        std::shared_ptr<GlobalViewModel> _model;
        GlobalViewOptions _options;

        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::ComboBox> _comboBox;
        std::shared_ptr<dtk::ColorWidget> _solidColorWidget;
        std::shared_ptr<dtk::ColorWidget> _checkersColor0Widget;
        std::shared_ptr<dtk::ColorWidget> _checkersColor1Widget;
        std::shared_ptr<dtk::IntEditSlider> _checkersSizeSlider;

        std::shared_ptr<dtk::ValueObserver<GlobalViewOptions> > _optionsObserver;
    };
}

