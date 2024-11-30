// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "JSONTool.h"

#include "App.h"
#include "FilesModel.h"
#include "SelectionModel.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Spacer.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/String.h>

namespace toucan
{
    void JSONWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::JSONWidget", parent);

        _item = item;

        _text = dtk::split(item->to_json_string(), { '\n' });

        _label = dtk::Label::create(context);
        _label->setMarginRole(dtk::SizeRole::MarginSmall);

        _bellows = dtk::Bellows::create(context, item->name(), shared_from_this());
        _bellows->setWidget(_label);
        _bellows->setOpen(true);

        _textUpdate();
    }

    JSONWidget::~JSONWidget()
    {}

    std::shared_ptr<JSONWidget> JSONWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<JSONWidget>(new JSONWidget);
        out->_init(context, item, parent);
        return out;
    }

    void JSONWidget::setOpen(bool value)
    {
        _bellows->setOpen(value);
    }

    void JSONWidget::setFilter(const std::string& value)
    {
        if (value == _filter)
            return;
        _filter = value;
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
        if (!_filter.empty())
        {
            std::vector<std::string> text;
            for (const auto& line : _text)
            {
                if (dtk::contains(line, _filter, dtk::CaseCompare::Insensitive))
                {
                    text.push_back(line);
                }
            }
            _label->setText(dtk::join(text, '\n'));
        }
        else
        {
            _label->setText(dtk::join(_text, '\n'));
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

        _nothingSelectedLabel = dtk::Label::create(context, "Nothing selected", _scrollLayout);
        _nothingSelectedLabel->setMarginRole(dtk::SizeRole::MarginSmall);

        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _bottomLayout = dtk::HorizontalLayout::create(context, _layout);
        _bottomLayout->setMarginRole(dtk::SizeRole::MarginInside);
        _bottomLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        _searchBox = dtk::SearchBox::create(context, _bottomLayout);
        _searchBox->setHStretch(dtk::Stretch::Expanding);
        _searchBox->setTooltip("Filter the JSON text");

        auto hLayout = dtk::HorizontalLayout::create(context, _bottomLayout);
        hLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
        auto openButton = dtk::ToolButton::create(context, hLayout);
        openButton->setMarginRole(dtk::SizeRole::MarginSmall);
        openButton->setIcon("BellowsOpen");
        openButton->setTooltip("Open all");
        auto closeButton = dtk::ToolButton::create(context, hLayout);
        closeButton->setMarginRole(dtk::SizeRole::MarginSmall);
        closeButton->setIcon("BellowsClosed");
        closeButton->setTooltip("Close all");

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

        _searchBox->setCallback(
            [this](const std::string& text)
            {
                for (const auto& widget : _widgets)
                {
                    widget->setFilter(text);
                }
            });

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                if (file)
                {
                    _selectionObserver = dtk::ListObserver<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >::create(
                        file->getSelectionModel()->observeSelection(),
                        [this](const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& selection)
                        {
                            for (const auto& widget : _widgets)
                            {
                                widget->setParent(nullptr);
                            }
                            _widgets.clear();
                            auto context = getContext();
                            for (const auto& item : selection)
                            {
                                auto widget = JSONWidget::create(context, item, _scrollLayout);
                                widget->setFilter(_searchBox->getText());
                                _widgets.push_back(widget);
                            }
                            _nothingSelectedLabel->setVisible(selection.empty());
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
