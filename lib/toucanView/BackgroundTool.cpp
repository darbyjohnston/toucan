// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "BackgroundTool.h"

#include "App.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/Spacer.h>
#include <dtk/core/String.h>

namespace toucan
{
    void BackgroundTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::BackgroundTool", "Background", parent);

        _model = app->getGlobalViewModel();

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, shared_from_this());
        _scrollWidget->setBorder(false);

        _layout = dtk::VerticalLayout::create(context);
        _layout->setMarginRole(dtk::SizeRole::MarginSmall);
        _layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
        _scrollWidget->setWidget(_layout);

        _comboBox = dtk::ComboBox::create(context, getViewBackgroundLabels(), _layout);
        
        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _solidColorWidget = dtk::ColorWidget::create(context, _layout);

        _checkersColor0Widget = dtk::ColorWidget::create(context, _layout);

        _checkersColor1Widget = dtk::ColorWidget::create(context, _layout);

        _checkersSizeSlider = dtk::IntEditSlider::create(context, _layout);
        _checkersSizeSlider->setRange(dtk::RangeI(10, 100));

        _comboBox->setIndexCallback(
            [this](int value)
            {
                if (_model)
                {
                    auto options = _model->getOptions();
                    options.background = static_cast<ViewBackground>(value);
                    _model->setOptions(options);
                }
            });

        _solidColorWidget->setCallback(
            [this](const dtk::Color4F& value)
            {
                if (_model)
                {
                    auto options = _model->getOptions();
                    options.solidColor = value;
                    _model->setOptions(options);
                }
            });

        _checkersColor0Widget->setCallback(
            [this](const dtk::Color4F& value)
            {
                if (_model)
                {
                    auto options = _model->getOptions();
                    options.checkersColor0 = value;
                    _model->setOptions(options);
                }
            });

        _checkersColor1Widget->setCallback(
            [this](const dtk::Color4F& value)
            {
                if (_model)
                {
                    auto options = _model->getOptions();
                    options.checkersColor1 = value;
                    _model->setOptions(options);
                }
            });

        _checkersSizeSlider->setCallback(
            [this](int value)
            {
                if (_model)
                {
                    auto options = _model->getOptions();
                    options.checkersSize = value;
                    _model->setOptions(options);
                }
            });

        _optionsObserver = dtk::ValueObserver<GlobalViewOptions>::create(
            _model->observeOptions(),
            [this](const GlobalViewOptions& value)
            {
                _options = value;
                _widgetUpdate();
            });
    }

    BackgroundTool::~BackgroundTool()
    {}

    std::shared_ptr<BackgroundTool> BackgroundTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<BackgroundTool>(new BackgroundTool);
        out->_init(context, app, parent);
        return out;
    }

    void BackgroundTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _scrollWidget->setGeometry(value);
    }

    void BackgroundTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_scrollWidget->getSizeHint());
    }

    void BackgroundTool::_widgetUpdate()
    {
        _comboBox->setCurrentIndex(static_cast<int>(_options.background));

        _solidColorWidget->setVisible(ViewBackground::Solid == _options.background);
        _solidColorWidget->setColor(_options.solidColor);

        _checkersColor0Widget->setVisible(ViewBackground::Checkers == _options.background);
        _checkersColor0Widget->setColor(_options.checkersColor0);

        _checkersColor1Widget->setVisible(ViewBackground::Checkers == _options.background);
        _checkersColor1Widget->setColor(_options.checkersColor1);

        _checkersSizeSlider->setVisible(ViewBackground::Checkers == _options.background);
        _checkersSizeSlider->setValue(_options.checkersSize);
    }
}
