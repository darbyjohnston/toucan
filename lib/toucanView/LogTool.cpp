// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "LogTool.h"

#include "App.h"
#include "FilesModel.h"
#include "SelectionModel.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Spacer.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/String.h>

namespace toucan
{
    namespace
    {
        const size_t textMax = 200;
    }

    void LogTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::LogTool", "Log", parent);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, _layout);
        _scrollWidget->setBorder(false);
        _scrollWidget->setVStretch(dtk::Stretch::Expanding);

        _label = dtk::Label::create(context);
        _label->setVAlign(dtk::VAlign::Top);
        _label->setFontRole(dtk::FontRole::Mono);
        _label->setMarginRole(dtk::SizeRole::MarginSmall);
        _scrollWidget->setWidget(_label);

        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _bottomLayout = dtk::HorizontalLayout::create(context, _layout);
        _bottomLayout->setMarginRole(dtk::SizeRole::MarginSmall);
        _bottomLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        _searchBox = dtk::SearchBox::create(context, _bottomLayout);
        _searchBox->setHStretch(dtk::Stretch::Expanding);
        _searchBox->setTooltip("Search the log");

        auto hLayout = dtk::HorizontalLayout::create(context, _bottomLayout);
        hLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
        auto clearButton = dtk::ToolButton::create(context, hLayout);
        clearButton->setIcon("Clear");
        clearButton->setTooltip("Clear the log");

        _textUpdate();

        clearButton->setClickedCallback(
            [this]
            {
                _text.clear();
                _textUpdate();
            });

        _searchBox->setCallback(
            [this](const std::string&)
            {
                _textUpdate();
            });

        _logObserver = dtk::ListObserver<dtk::LogItem>::create(
            context->getSystem<dtk::LogSystem>()->observeLogItems(),
            [this](const std::vector<dtk::LogItem>& items)
            {
                for (const auto& item : items)
                {
                    for (const auto& line : dtk::split(dtk::toString(item), '\n'))
                    {
                        _text.push_back(line);
                    }
                }
                _textUpdate();
            });
    }

    LogTool::~LogTool()
    {}

    std::shared_ptr<LogTool> LogTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<LogTool>(new LogTool);
        out->_init(context, app, parent);
        return out;
    }

    void LogTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void LogTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void LogTool::_textUpdate()
    {
        while (_text.size() > textMax)
        {
            _text.pop_front();
        }

        std::vector<std::string> text;
        const std::string search = _searchBox->getText();
        if (search.empty())
        {
            text.insert(text.begin(), _text.begin(), _text.end());
        }
        else
        {
            for (const auto& line : _text)
            {
                if (dtk::contains(line, search, dtk::CaseCompare::Insensitive))
                {
                    text.push_back(line);
                }
            }
        }
        _label->setText(dtk::join(text, '\n'));
    }
}
