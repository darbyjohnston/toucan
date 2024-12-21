// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ViewToolBar.h"

#include "App.h"
#include "FilesModel.h"
#include "ViewModel.h"

namespace toucan
{
    void ViewToolBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::map<std::string, std::shared_ptr<dtk::Action> >& actions,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::ViewToolBar", parent);

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        auto hLayout = dtk::HorizontalLayout::create(context, _layout);
        hLayout->setSpacingRole(dtk::SizeRole::SpacingTool);
        std::vector<std::string> actionNames =
        {
            "View/ZoomIn",
            "View/ZoomOut",
            "View/ZoomReset"
        };
        for (const auto& name : actionNames)
        {
            auto i = actions.find(name);
            auto button = dtk::ToolButton::create(context, hLayout);
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
            _buttons[name] = button;
        }
        
        auto i = actions.find("View/Frame");
        auto button = dtk::ToolButton::create(context, hLayout);
        button->setIcon(i->second->icon);
        button->setCheckable(true);
        button->setTooltip(i->second->toolTip);
        button->setCheckedCallback(
            [i](bool value)
            {
                if (i->second->checkedCallback)
                {
                    i->second->checkedCallback(value);
                }
            });
        _buttons["View/Frame"] = button;

        _widgetUpdate();

        _filesObserver = dtk::ListObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeFiles(),
            [this](const std::vector<std::shared_ptr<File> >& files)
            {
                _filesSize = files.size();
                _widgetUpdate();
            });
    }

    ViewToolBar::~ViewToolBar()
    {}

    std::shared_ptr<ViewToolBar> ViewToolBar::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::map<std::string, std::shared_ptr<dtk::Action> >& actions,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<ViewToolBar>(new ViewToolBar);
        out->_init(context, app, actions, parent);
        return out;
    }

    void ViewToolBar::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void ViewToolBar::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void ViewToolBar::_widgetUpdate()
    {
        _buttons["View/ZoomIn"]->setEnabled(_filesSize > 0);
        _buttons["View/ZoomOut"]->setEnabled(_filesSize > 0);
        _buttons["View/ZoomReset"]->setEnabled(_filesSize > 0);
        _buttons["View/Frame"]->setEnabled(_filesSize > 0);

        if (_file)
        {
            _frameViewObserver = dtk::ValueObserver<bool>::create(
                _file->getViewModel()->observeFrameView(),
                [this](bool value)
                {
                    _buttons["View/Frame"]->setChecked(value);
                });
        }
        else
        {
            _frameViewObserver.reset();
        }
    }
}
