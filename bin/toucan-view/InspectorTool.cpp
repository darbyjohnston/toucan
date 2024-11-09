// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "InspectorTool.h"

#include "App.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/Spacer.h>
#include <dtk/ui/ToolButton.h>

namespace toucan
{
    void InspectorWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::InspectorWidget", parent);

        _item = item;

        std::string text = item->to_json_string();
        auto label = dtk::Label::create(context, text);
        label->setMarginRole(dtk::SizeRole::MarginSmall);

        _bellows = dtk::Bellows::create(context, item->name(), shared_from_this());
        _bellows->setWidget(label);
        _bellows->setOpen(true);
    }

    InspectorWidget::~InspectorWidget()
    {}

    std::shared_ptr<InspectorWidget> InspectorWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<InspectorWidget>(new InspectorWidget);
        out->_init(context, item, parent);
        return out;
    }

    void InspectorWidget::setOpen(bool value)
    {
        _bellows->setOpen(value);
    }

    void InspectorWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _bellows->setGeometry(value);
    }

    void InspectorWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_bellows->getSizeHint());
    }

    void InspectorTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::InspectorTool", "Inspector", parent);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        auto hLayout = dtk::HorizontalLayout::create(context, _layout);
        hLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
        auto spacer = dtk::Spacer::create(context, dtk::Orientation::Horizontal, hLayout);
        spacer->setSpacingRole(dtk::SizeRole::None);
        spacer->setStretch(dtk::Stretch::Expanding);
        auto openButton = dtk::ToolButton::create(context, hLayout);
        openButton->setMarginRole(dtk::SizeRole::MarginSmall);
        openButton->setIcon("BellowsOpen");
        openButton->setTooltip("Open all");
        auto closeButton = dtk::ToolButton::create(context, hLayout);
        closeButton->setMarginRole(dtk::SizeRole::MarginSmall);
        closeButton->setIcon("BellowsClosed");
        closeButton->setTooltip("Close all");

        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, _layout);
        _scrollWidget->setBorder(false);
        _scrollWidget->setVStretch(dtk::Stretch::Expanding);

        _scrollLayout = dtk::VerticalLayout::create(context);
        _scrollLayout->setSpacingRole(dtk::SizeRole::None);
        _scrollWidget->setWidget(_scrollLayout);

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

        _documentObserver = dtk::ValueObserver<std::shared_ptr<Document> >::create(
            app->getDocumentsModel()->observeCurrent(),
            [this](const std::shared_ptr<Document>& document)
            {
                if (document)
                {
                    _selectionObserver = dtk::ListObserver<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >::create(
                        document->getSelectionModel()->observeSelection(),
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
                                auto widget = InspectorWidget::create(context, item, _scrollLayout);
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

    InspectorTool::~InspectorTool()
    {}

    std::shared_ptr<InspectorTool> InspectorTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<InspectorTool>(new InspectorTool);
        out->_init(context, app, parent);
        return out;
    }

    void InspectorTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void InspectorTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }
}
