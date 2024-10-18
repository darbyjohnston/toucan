// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ExportTool.h"

namespace toucan
{
    void ExportWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::ExportWidget", parent);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
    }

    ExportWidget::~ExportWidget()
    {}

    std::shared_ptr<ExportWidget> ExportWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<ExportWidget>(new ExportWidget);
        out->_init(context, parent);
        return out;
    }

    void ExportWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void ExportWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void ExportTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::ExportTool", "Export", parent);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);
    }

    ExportTool::~ExportTool()
    {}

    std::shared_ptr<ExportTool> ExportTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<ExportTool>(new ExportTool);
        out->_init(context, app, parent);
        return out;
    }

    void ExportTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void ExportTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }
}
