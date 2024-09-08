// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "InspectorTool.h"

#include "App.h"

#include <dtk/ui/GridLayout.h>
#include <dtk/ui/Label.h>
#include <dtk/core/Format.h>

namespace toucan
{
    void InspectorWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::InspectorWidget", parent);

        _item = item;

        auto layout = dtk::GridLayout::create(context);
        layout->setMarginRole(dtk::SizeRole::MarginSmall);
        layout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        int row = 0;
        auto opt = item->trimmed_range_in_parent();
        if (opt.has_value())
        {
            OTIO_NS::TimeRange range = opt.value();
            auto label = dtk::Label::create(context, "Range:", layout);
            layout->setGridPos(label, row, 0);
            std::string text = dtk::Format("{0}-{1}@{2}").
                arg(range.start_time().value()).
                arg(range.end_time_inclusive().value()).
                arg(range.duration().rate());
            label = dtk::Label::create(context, text, layout);
            layout->setGridPos(label, row, 1);
            ++row;
        }

        auto label = dtk::Label::create(context, "Item range:", layout);
        layout->setGridPos(label, row, 0);
        OTIO_NS::TimeRange range = item->trimmed_range();
        std::string text = dtk::Format("{0}-{1}@{2}").
            arg(range.start_time().value()).
            arg(range.end_time_inclusive().value()).
            arg(range.duration().rate());
        label = dtk::Label::create(context, text, layout);
        layout->setGridPos(label, row, 1);
        ++row;

        _bellows = dtk::Bellows::create(context, item->name(), shared_from_this());
        _bellows->setWidget(layout);
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

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, shared_from_this());

        _layout = dtk::VerticalLayout::create(context);
        _layout->setSpacingRole(dtk::SizeRole::None);
        _scrollWidget->setWidget(_layout);

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
                            auto context = _getContext().lock();
                            for (const auto& item : selection)
                            {
                                auto widget = InspectorWidget::create(context, item, _layout);
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
        _scrollWidget->setGeometry(value);
    }

    void InspectorTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_scrollWidget->getSizeHint());
    }
}
