// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FileTab.h"

#include "App.h"
#include "Viewport.h"

namespace toucan
{
    void FileTab::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<File>& file,
        const std::shared_ptr<IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::FileTab", parent);
        _viewport = Viewport::create(context, file, shared_from_this());
        _viewport->setStretch(dtk::Stretch::Expanding);
    }

    FileTab::~FileTab()
    {}

    std::shared_ptr<FileTab> FileTab::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<File>& file,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<FileTab>(new FileTab);
        out->_init(context, app, file, parent);
        return out;
    }

    void FileTab::setGeometry(const dtk::Box2I& value)
    {
        dtk::IWidget::setGeometry(value);
        _viewport->setGeometry(value);
    }

    void FileTab::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        dtk::IWidget::sizeHintEvent(event);
        _setSizeHint(_viewport->getSizeHint());
    }
}
