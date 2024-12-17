// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "JSONTool.h"

#include "App.h"
#include "FilesModel.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Spacer.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/Format.h>
#include <dtk/core/String.h>

namespace toucan
{
    void JSONWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata>& object,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::JSONWidget", parent);

        _object = object;
        _text = dtk::split(object->to_json_string(), { '\n' });
        for (int i = 0; i < _text.size(); ++i)
        {
            _lineNumbers.push_back(dtk::Format("{0}").arg(i, dtk::digits(_text.size()), '0'));
        }

        _bellows = dtk::Bellows::create(context, object->name(), shared_from_this());
        _bellows->setOpen(true);

        auto hLayout = dtk::HorizontalLayout::create(context);
        hLayout->setSpacingRole(dtk::SizeRole::None);
        _bellows->setWidget(hLayout);

        _lineNumbersLabel = dtk::Label::create(context, hLayout);
        _lineNumbersLabel->setBackgroundRole(dtk::ColorRole::Base);
        _lineNumbersLabel->setFontRole(dtk::FontRole::Mono);
        _lineNumbersLabel->setMarginRole(dtk::SizeRole::MarginSmall);
        _lineNumbersLabel->setHStretch(dtk::Stretch::Fixed);

        _textLabel = dtk::Label::create(context, hLayout);
        _textLabel->setFontRole(dtk::FontRole::Mono);
        _textLabel->setMarginRole(dtk::SizeRole::MarginSmall);
        _textLabel->setHStretch(dtk::Stretch::Expanding);

        _textUpdate();
    }

    JSONWidget::~JSONWidget()
    {}

    std::shared_ptr<JSONWidget> JSONWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata>& object,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<JSONWidget>(new JSONWidget);
        out->_init(context, object, parent);
        return out;
    }

    void JSONWidget::setOpen(bool value)
    {
        _bellows->setOpen(value);
    }

    void JSONWidget::setSearch(const std::string& value)
    {
        if (value == _search)
            return;
        _search = value;
        _textUpdate();
    }

    void JSONWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _bellows->setGeometry(value);
    }

    void JSONWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_bellows->getSizeHint());
    }

    void JSONWidget::_textUpdate()
    {
        if (!_search.empty())
        {
            std::vector<std::string> lineNumbers;
            std::vector<std::string> text;
            for (size_t i = 0; i < _lineNumbers.size() && i < _text.size(); ++i)
            {
                if (dtk::contains(_text[i], _search, dtk::CaseCompare::Insensitive))
                {
                    lineNumbers.push_back(_lineNumbers[i]);
                    text.push_back(_text[i]);
                }
            }
            _lineNumbersLabel->setText(dtk::join(lineNumbers, '\n'));
            _textLabel->setText(dtk::join(text, '\n'));
        }
        else
        {
            _lineNumbersLabel->setText(dtk::join(_lineNumbers, '\n'));
            _textLabel->setText(dtk::join(_text, '\n'));
        }
    }

    void JSONTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::JSONTool", "JSON", parent);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, _layout);
        _scrollWidget->setBorder(false);
        _scrollWidget->setVStretch(dtk::Stretch::Expanding);

        _scrollLayout = dtk::VerticalLayout::create(context);
        _scrollLayout->setSpacingRole(dtk::SizeRole::None);
        _scrollWidget->setWidget(_scrollLayout);

        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _bottomLayout = dtk::HorizontalLayout::create(context, _layout);
        _bottomLayout->setMarginRole(dtk::SizeRole::MarginSmall);
        _bottomLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        _searchBox = dtk::SearchBox::create(context, _bottomLayout);
        _searchBox->setHStretch(dtk::Stretch::Expanding);
        _searchBox->setTooltip("Search the JSON text");

        auto hLayout = dtk::HorizontalLayout::create(context, _bottomLayout);
        hLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
        auto openButton = dtk::ToolButton::create(context, hLayout);
        openButton->setIcon("BellowsOpen");
        openButton->setTooltip("Open all");
        auto closeButton = dtk::ToolButton::create(context, hLayout);
        closeButton->setIcon("BellowsClosed");
        closeButton->setTooltip("Close all");

        _searchBox->setCallback(
            [this](const std::string& text)
            {
                for (const auto& widget : _widgets)
                {
                    widget->setSearch(text);
                }
            });

        openButton->setClickedCallback(
            [this]
            {
                for (const auto& widget : _widgets)
                {
                    widget->setOpen(true);
                }
            });

        closeButton->setClickedCallback(
            [this]
            {
                for (const auto& widget : _widgets)
                {
                    widget->setOpen(false);
                }
            });

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                if (file)
                {
                    _selectionObserver = dtk::ListObserver<SelectionItem>::create(
                        file->getSelectionModel()->observeSelection(),
                        [this](const std::vector<SelectionItem>& selection)
                        {
                            for (const auto& widget : _widgets)
                            {
                                widget->setParent(nullptr);
                            }
                            _widgets.clear();
                            auto context = getContext();
                            for (const auto& item : selection)
                            {
                                auto widget = JSONWidget::create(context, item.object, _scrollLayout);
                                widget->setSearch(_searchBox->getText());
                                _widgets.push_back(widget);
                            }
                        });
                }
                else
                {
                    for (const auto& widget : _widgets)
                    {
                        widget->setParent(nullptr);
                    }
                    _widgets.clear();
                    _selectionObserver.reset();
                }
            });
    }

    JSONTool::~JSONTool()
    {}

    std::shared_ptr<JSONTool> JSONTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<JSONTool>(new JSONTool);
        out->_init(context, app, parent);
        return out;
    }

    void JSONTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void JSONTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }
}
