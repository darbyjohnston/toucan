// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/FilesModel.h>
#include <toucanView/IToolWidget.h>

#include <dtk/ui/CheckBox.h>
#include <dtk/ui/ComboBox.h>
#include <dtk/ui/GridLayout.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/ObservableList.h>

namespace toucan
{
    class File;

    //! Compare widget.
    class CompareWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<File>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~CompareWidget();

        //! Create a new widget.
        static std::shared_ptr<CompareWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<File>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setCurrent(bool);
        void setCurrentCallback(const std::function<void(void)>&);

        void setB(bool);
        void setBCallback(const std::function<void(bool)>&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        bool _current = false;
        bool _b = false;

        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::shared_ptr<dtk::ToolButton> _button;
        std::shared_ptr<dtk::ToolButton> _bButton;
    };

    //! Compare tool.
    class CompareTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~CompareTool();

        //! Create a new tool.
        static std::shared_ptr<CompareTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _widgetUpdate();
        void _indexUpdate();

        std::weak_ptr<App> _app;
        std::vector<std::shared_ptr<File> > _files;
        int _currentIndex = -1;
        int _bIndex = -1;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<dtk::VerticalLayout> _widgetLayout;
        std::vector<std::shared_ptr<CompareWidget> > _widgets;
        std::shared_ptr<dtk::GridLayout> _bottomLayout;
        std::shared_ptr<dtk::ComboBox> _modeComboBox;
        std::shared_ptr<dtk::ComboBox> _timeComboBox;
        std::shared_ptr<dtk::CheckBox> _fitSizeCheckBox;

        std::shared_ptr<dtk::ListObserver<std::shared_ptr<File> > > _filesObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _fileIndexObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _bIndexObserver;
        std::shared_ptr<dtk::ValueObserver<CompareOptions> > _compareOptionsObserver;
    };
}

