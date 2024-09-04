// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ToolBar.h"

#include "App.h"

namespace toucan
{
    void ToolBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::map<std::string, std::shared_ptr<dtk::Action> >& actions,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::ToolBar", parent);

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        auto i = actions.find("FileOpen");
        auto button = dtk::ToolButton::create(context, _layout);
        button->setIcon(i->second->icon);
        button->setTooltip(i->second->toolTip);
        button->setClickedCallback(
            [i]
            {
                if (i->second->callback)
                {
                    i->second->callback();
                }
            });

        i = actions.find("FileClose");
        button = dtk::ToolButton::create(context, _layout);
        button->setIcon(i->second->icon);
        button->setTooltip(i->second->toolTip);
        button->setClickedCallback(
            [i]
            {
                if (i->second->callback)
                {
                    i->second->callback();
                }
            });
    }

    ToolBar::~ToolBar()
    {}

    std::shared_ptr<ToolBar> ToolBar::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::map<std::string, std::shared_ptr<dtk::Action> >& actions,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<ToolBar>(new ToolBar);
        out->_init(context, app, actions, parent);
        return out;
    }

    void ToolBar::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void ToolBar::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }
}
