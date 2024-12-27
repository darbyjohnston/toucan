// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "CompareTool.h"

#include "App.h"
#include "SelectionModel.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/Spacer.h>
#include <dtk/core/String.h>

namespace toucan
{
    void CompareWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<File>& file,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::CompareWidget", parent);

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _button = dtk::ToolButton::create(context, _layout);
        std::string text = file->getPath().filename().string();
        _button->setText(dtk::elide(text));
        _button->setHStretch(dtk::Stretch::Expanding);
        _button->setTooltip(text);

        _bButton = dtk::ToolButton::create(context, _layout);
        _bButton->setText("B");
        _bButton->setCheckable(true);
        _bButton->setTooltip("Set the B file for comparison");
    }

    CompareWidget::~CompareWidget()
    {}

    std::shared_ptr<CompareWidget> CompareWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<File>& file,
        const std::shared_ptr<IWidget>& parent)
    {
        std::shared_ptr<CompareWidget> out(new CompareWidget);
        out->_init(context, file, parent);
        return out;
    }

    void CompareWidget::setCurrent(bool value)
    {
        _button->setButtonRole(value ? dtk::ColorRole::Checked : dtk::ColorRole::None);
    }

    void CompareWidget::setCurrentCallback(const std::function<void(void)>& callback)
    {
        _button->setClickedCallback(
            [callback]
            {
                callback();
            });
    }

    void CompareWidget::setB(bool value)
    {
        _bButton->setChecked(value);
    }

    void CompareWidget::setBCallback(const std::function<void(bool)>& value)
    {
        _bButton->setCheckedCallback(value);
    }

    void CompareWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void CompareWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void CompareTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::CompareTool", "Compare", parent);

        _app = app;

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, _layout);
        _scrollWidget->setBorder(false);
        _scrollWidget->setVStretch(dtk::Stretch::Expanding);

        _widgetLayout = dtk::VerticalLayout::create(context);
        _widgetLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
        _scrollWidget->setWidget(_widgetLayout);

        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _bottomLayout = dtk::GridLayout::create(context, _layout);
        _bottomLayout->setMarginRole(dtk::SizeRole::MarginSmall);
        _bottomLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        auto label = dtk::Label::create(context, "Mode:", _bottomLayout);
        _bottomLayout->setGridPos(label, 0, 0);
        _modeComboBox = dtk::ComboBox::create(context, getCompareModeLabels(), _bottomLayout);
        _modeComboBox->setHStretch(dtk::Stretch::Expanding);
        _modeComboBox->setTooltip("Set the comparison mode");
        _bottomLayout->setGridPos(_modeComboBox, 0, 1);

        label = dtk::Label::create(context, "Match start time:", _bottomLayout);
        _bottomLayout->setGridPos(label, 1, 0);
        _matchStartTimeCheckBox = dtk::CheckBox::create(context, _bottomLayout);
        _matchStartTimeCheckBox->setHStretch(dtk::Stretch::Expanding);
        _matchStartTimeCheckBox->setTooltip("Adjust the B file start time to match the A file");
        _bottomLayout->setGridPos(_matchStartTimeCheckBox, 1, 1);

        label = dtk::Label::create(context, "Fit size:", _bottomLayout);
        _bottomLayout->setGridPos(label, 2, 0);
        _fitSizeCheckBox = dtk::CheckBox::create(context, _bottomLayout);
        _fitSizeCheckBox->setHStretch(dtk::Stretch::Expanding);
        _fitSizeCheckBox->setTooltip("Fit the B image size to match the A image");
        _bottomLayout->setGridPos(_fitSizeCheckBox, 2, 1);

        _modeComboBox->setIndexCallback(
            [this](int value)
            {
                auto app = _app.lock();
                CompareOptions options = app->getFilesModel()->getCompareOptions();
                options.mode = static_cast<CompareMode>(value);
                app->getFilesModel()->setCompareOptions(options);
            });

        _matchStartTimeCheckBox->setCheckedCallback(
            [this](bool value)
            {
                auto app = _app.lock();
                CompareOptions options = app->getFilesModel()->getCompareOptions();
                options.matchStartTime = value;
                app->getFilesModel()->setCompareOptions(options);
            });

        _fitSizeCheckBox->setCheckedCallback(
            [this](bool value)
            {
                auto app = _app.lock();
                CompareOptions options = app->getFilesModel()->getCompareOptions();
                options.fitSize = value;
                app->getFilesModel()->setCompareOptions(options);
            });

        _filesObserver = dtk::ListObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeFiles(),
            [this](const std::vector<std::shared_ptr<File> >& value)
            {
                _files = value;
                _widgetUpdate();
                _indexUpdate();
            });

        _fileIndexObserver = dtk::ValueObserver<int>::create(
            app->getFilesModel()->observeCurrentIndex(),
            [this](int value)
            {
                _currentIndex = value;
                _indexUpdate();
            });

        _bIndexObserver = dtk::ValueObserver<int>::create(
            app->getFilesModel()->observeBIndex(),
            [this](int value)
            {
                _bIndex = value;
                _indexUpdate();
            });
        
        _compareOptionsObserver = dtk::ValueObserver<CompareOptions>::create(
            app->getFilesModel()->observeCompareOptions(),
            [this](const CompareOptions& value)
            {
                _modeComboBox->setCurrentIndex(static_cast<int>(value.mode));
                _matchStartTimeCheckBox->setChecked(value.matchStartTime);
                _fitSizeCheckBox->setChecked(value.fitSize);
            });
    }

    CompareTool::~CompareTool()
    {}

    std::shared_ptr<CompareTool> CompareTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<CompareTool>(new CompareTool);
        out->_init(context, app, parent);
        return out;
    }

    void CompareTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void CompareTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void CompareTool::_widgetUpdate()
    {
        for (const auto& widget : _widgets)
        {
            widget->setParent(nullptr);
        }
        _widgets.clear();

        auto context = getContext();
        for (size_t i = 0; i < _files.size(); ++i)
        {
            auto widget = CompareWidget::create(context, _files[i], _widgetLayout);
            widget->setCurrent(i == _currentIndex);
            widget->setB(i == _bIndex);
            widget->setCurrentCallback(
                [this, i]
                {
                    auto app = _app.lock();
                    app->getFilesModel()->setCurrentIndex(i);
                });
            widget->setBCallback(
                [this, i](bool value)
                {
                    auto app = _app.lock();
                    app->getFilesModel()->setBIndex(value ? i : -1);
                });
            _widgets.push_back(widget);
        }
    }

    void CompareTool::_indexUpdate()
    {
        for (size_t i = 0; i < _widgets.size(); ++i)
        {
            _widgets[i]->setCurrent(i == _currentIndex);
            _widgets[i]->setB(i == _bIndex);
        }
    }
}
