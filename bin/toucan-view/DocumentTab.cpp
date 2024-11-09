// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "DocumentTab.h"

#include "App.h"
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
        _viewport = Viewport::create(context, document, shared_from_this());
        _viewport->setStretch(dtk::Stretch::Expanding);
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
        _viewport->setGeometry(value);
    }

    void DocumentTab::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        dtk::IWidget::sizeHintEvent(event);
        _setSizeHint(_viewport->getSizeHint());
    }
}
