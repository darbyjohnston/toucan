// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "InfoTool.h"

#include "App.h"
#include "FilesModel.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Spacer.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/Format.h>
#include <dtk/core/String.h>

#include <opentimelineio/marker.h>

namespace toucan
{
    void InfoItemWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata>& object,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::InfoItemWidget", parent);

        _object = object;

        _bellows = dtk::Bellows::create(context, object->name(), shared_from_this());
        _bellows->setOpen(true);

        _layout = dtk::GridLayout::create(context);
        _layout->setRowBackgroundRole(dtk::ColorRole::Base);
        _layout->setSpacingRole(dtk::SizeRole::None);
        _bellows->setWidget(_layout);

        _text.push_back(std::make_pair("Schema:", object->schema_name()));
        _text.push_back(std::make_pair("Name:", object->name()));

        if (auto item = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(object))
        {
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
        }
        else if (auto marker = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Marker>(object))
        {
            _text.push_back(std::make_pair("Color:", marker->color()));

            OTIO_NS::TimeRange timeRange = marker->marked_range();
            std::string text = dtk::Format("{0} @ {1} / {2} @ {3}").
                arg(timeRange.start_time().value()).
                arg(timeRange.start_time().rate()).
                arg(timeRange.duration().value()).
                arg(timeRange.duration().rate());
            _text.push_back(std::make_pair("Range:", text));

            _text.push_back(std::make_pair("Comment:", marker->comment()));
        }

        int row = 0;
        for (const auto& text : _text)
        {
            auto label = dtk::Label::create(context, text.first, _layout);
            label->setMarginRole(dtk::SizeRole::MarginSmall);
            _layout->setGridPos(label, row, 0);
            std::shared_ptr<dtk::Label> label2;
            if (!text.second.empty())
            {
                label2 = dtk::Label::create(context, text.second, _layout);
                label2->setMarginRole(dtk::SizeRole::MarginSmall);
                _layout->setGridPos(label2, row, 1);
            }
            _labels.push_back(std::make_pair(label, label2));
            ++row;
        }

        _textUpdate();
    }

    InfoItemWidget::~InfoItemWidget()
    {}

    std::shared_ptr<InfoItemWidget> InfoItemWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata>& object,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<InfoItemWidget>(new InfoItemWidget);
        out->_init(context, object, parent);
        return out;
    }

    void InfoItemWidget::setOpen(bool value)
    {
        _bellows->setOpen(value);
    }

    void InfoItemWidget::setSearch(const std::string& value)
    {
        if (value == _search)
            return;
        _search = value;
        _textUpdate();
    }

    void InfoItemWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _bellows->setGeometry(value);
    }

    void InfoItemWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_bellows->getSizeHint());
    }

    void InfoItemWidget::_textUpdate()
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
            if (_labels[i].second)
            {
                _labels[i].second->setVisible(visible);
            }
        }
    }

    void InfoTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::InfoTool", "Info", parent);

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
                                auto widget = InfoItemWidget::create(context, item.object, _scrollLayout);
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

    InfoTool::~InfoTool()
    {}

    std::shared_ptr<InfoTool> InfoTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<InfoTool>(new InfoTool);
        out->_init(context, app, parent);
        return out;
    }

    void InfoTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void InfoTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }
}
