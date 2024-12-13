// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "MarkersTool.h"

#include "App.h"
#include "FilesModel.h"
#include "SelectionModel.h"

#include <dtk/ui/Divider.h>
#include <dtk/ui/Spacer.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/String.h>

namespace toucan
{
    void MarkersTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::MarkersTool", "Markers", parent);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, _layout);
        _scrollWidget->setBorder(false);
        _scrollWidget->setVStretch(dtk::Stretch::Expanding);

        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _bottomLayout = dtk::HorizontalLayout::create(context, _layout);
        _bottomLayout->setMarginRole(dtk::SizeRole::MarginInside);
        _bottomLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        _searchBox = dtk::SearchBox::create(context, _bottomLayout);
        _searchBox->setHStretch(dtk::Stretch::Expanding);
        _searchBox->setTooltip("Search the markers");

        _searchBox->setCallback(
            [this](const std::string& text)
            {
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
                        });
                }
                else
                {
                    _selectionObserver.reset();
                }
            });
    }

    MarkersTool::~MarkersTool()
    {}

    std::shared_ptr<MarkersTool> MarkersTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<MarkersTool>(new MarkersTool);
        out->_init(context, app, parent);
        return out;
    }

    void MarkersTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void MarkersTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }
}
