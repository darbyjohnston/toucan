// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "DetailsTool.h"

#include "App.h"
#include "FilesModel.h"
#include "SelectionModel.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Spacer.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/Format.h>
#include <dtk/core/String.h>

namespace toucan
{
    void DetailsItemWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::DetailsItemWidget", parent);

        _item = item;

        _bellows = dtk::Bellows::create(context, item->name(), shared_from_this());
        _bellows->setOpen(true);

        _layout = dtk::GridLayout::create(context);
        _layout->setMarginRole(dtk::SizeRole::MarginSmall);
        _layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
        _bellows->setWidget(_layout);

        _text.push_back(std::make_pair("Name:", item->name()));

        _text.push_back(std::make_pair(
            "Enabled:",
            dtk::Format("{0}").arg(item->enabled())));

        std::string text;
        if (item->source_range().has_value())
        {
            OTIO_NS::TimeRange timeRange = item->source_range().value();
            text = dtk::Format("{0} @ {1} / {2} @ {3}").
                arg(timeRange.start_time().value()).
                arg(timeRange.start_time().rate()).
                arg(timeRange.duration().value()).
                arg(timeRange.duration().rate());
        }
        _text.push_back(std::make_pair("Source range:", text));

        OTIO_NS::TimeRange timeRange = item->available_range();
        text = dtk::Format("{0} @ {1} / {2} @ {3}").
            arg(timeRange.start_time().value()).
            arg(timeRange.start_time().rate()).
            arg(timeRange.duration().value()).
            arg(timeRange.duration().rate());
        _text.push_back(std::make_pair("Available range:", text));

        timeRange = item->trimmed_range();
        text = dtk::Format("{0} @ {1} / {2} @ {3}").
            arg(timeRange.start_time().value()).
            arg(timeRange.start_time().rate()).
            arg(timeRange.duration().value()).
            arg(timeRange.duration().rate());
        _text.push_back(std::make_pair("Trimmed range:", text));

        text.clear();
        //! \todo Calling trimmed_range_in_parent() on a stack causes a crash?
        auto stack = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Stack>(item);
        if (!stack && item->trimmed_range_in_parent().has_value())
        {
            timeRange = item->trimmed_range_in_parent().value();
            text = dtk::Format("{0} @ {1} / {2} @ {3}").
                arg(timeRange.start_time().value()).
                arg(timeRange.start_time().rate()).
                arg(timeRange.duration().value()).
                arg(timeRange.duration().rate());
        }
        _text.push_back(std::make_pair("Trimmed range in parent:", text));

        int row = 0;
        for (const auto& text : _text)
        {
            auto label = dtk::Label::create(context, text.first, _layout);
            _layout->setGridPos(label, row, 0);
            auto label2 = dtk::Label::create(context, text.second, _layout);
            _layout->setGridPos(label2, row, 1);
            _labels.push_back(std::make_pair(label, label2));
            ++row;
        }

        _textUpdate();
    }

    DetailsItemWidget::~DetailsItemWidget()
    {}

    std::shared_ptr<DetailsItemWidget> DetailsItemWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<DetailsItemWidget>(new DetailsItemWidget);
        out->_init(context, item, parent);
        return out;
    }

    void DetailsItemWidget::setOpen(bool value)
    {
        _bellows->setOpen(value);
    }

    void DetailsItemWidget::setSearch(const std::string& value)
    {
        if (value == _search)
            return;
        _search = value;
        _textUpdate();
    }

    void DetailsItemWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _bellows->setGeometry(value);
    }

    void DetailsItemWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_bellows->getSizeHint());
    }

    void DetailsItemWidget::_textUpdate()
    {
        for (size_t i = 0; i < _text.size() && i < _labels.size(); ++i)
        {
            bool visible = true;
            if (!_search.empty())
            {
                visible &=
                    dtk::contains(_text[i].first, _search, dtk::CaseCompare::Insensitive) ||
                    dtk::contains(_text[i].second, _search, dtk::CaseCompare::Insensitive);
            }
            _labels[i].first->setVisible(visible);
            _labels[i].second->setVisible(visible);
        }
    }

    void DetailsTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::DetailsTool", "Details", parent);

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
                                auto widget = DetailsItemWidget::create(context, item, _scrollLayout);
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

    DetailsTool::~DetailsTool()
    {}

    std::shared_ptr<DetailsTool> DetailsTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<DetailsTool>(new DetailsTool);
        out->_init(context, app, parent);
        return out;
    }

    void DetailsTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void DetailsTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }
}
