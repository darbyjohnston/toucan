// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "DocumentTab.h"

#include "App.h"
#include "BottomBar.h"
#include "TimelineView.h"
#include "Viewport.h"

namespace toucan
{
    void DocumentTab::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Document>& document,
        const std::shared_ptr<IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::DocumentTab", parent);

        _vSplitter = dtk::Splitter::create(context, dtk::Orientation::Vertical, shared_from_this());
        _vSplitter->setSplit({ .7F, .3F });
        _vSplitter->setStretch(dtk::Stretch::Expanding);
        _hSplitter = dtk::Splitter::create(context, dtk::Orientation::Horizontal, _vSplitter);
        _hSplitter->setSplit({ .75F, .25F });

        _viewport = Viewport::create(context, document, _hSplitter);
        _viewport->setStretch(dtk::Stretch::Expanding);

        _bottomLayout = dtk::VerticalLayout::create(context, _vSplitter);
        _bottomLayout->setSpacingRole(dtk::SizeRole::None);
        _bottomBar = BottomBar::create(context, app, _bottomLayout);
        _timelineView = TimelineView::create(context, app, document, _bottomLayout);
        _timelineView->setVStretch(dtk::Stretch::Expanding);
    }

    DocumentTab::~DocumentTab()
    {}

    std::shared_ptr<DocumentTab> DocumentTab::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Document>& document,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<DocumentTab>(new DocumentTab);
        out->_init(context, app, document, parent);
        return out;
    }

    void DocumentTab::setGeometry(const dtk::Box2I& value)
    {
        dtk::IWidget::setGeometry(value);
        _vSplitter->setGeometry(value);
    }

    void DocumentTab::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        dtk::IWidget::sizeHintEvent(event);
        _setSizeHint(_vSplitter->getSizeHint());
    }
}
