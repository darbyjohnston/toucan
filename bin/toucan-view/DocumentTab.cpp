// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "DocumentTab.h"

#include "App.h"
#include "BottomBar.h"
#include "InspectorTool.h"
#include "TimelineWidget.h"
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
        _vSplitter->setSplit({ .6F, .4F });
        _vSplitter->setStretch(dtk::Stretch::Expanding);
        _hSplitter = dtk::Splitter::create(context, dtk::Orientation::Horizontal, _vSplitter);
        _hSplitter->setSplit({ .75F, .25F });

        _viewport = Viewport::create(context, document, _hSplitter);
        _viewport->setStretch(dtk::Stretch::Expanding);

        _toolWidget = dtk::TabWidget::create(context, _hSplitter);
        _toolWidgets.push_back(InspectorTool::create(context, app, document));
        for (const auto& toolWidget : _toolWidgets)
        {
            _toolWidget->addTab(toolWidget->getText(), toolWidget);
        }

        _bottomLayout = dtk::VerticalLayout::create(context, _vSplitter);
        _bottomLayout->setSpacingRole(dtk::SizeRole::None);
        _bottomBar = BottomBar::create(context, app, _bottomLayout);
        _timelineWidget = TimelineWidget::create(context, app, document, _bottomLayout);
        _timelineWidget->setVStretch(dtk::Stretch::Expanding);

        std::weak_ptr<App> appWeak(app);
        _controlsObserver = dtk::MapObserver<WindowControl, bool>::create(
            app->getWindowModel()->observeControls(),
            [this](const std::map<WindowControl, bool>& value)
            {
                auto i = value.find(WindowControl::TimelineWidget);
                _timelineWidget->setVisible(i->second);
                auto j = value.find(WindowControl::BottomBar);
                _bottomBar->setVisible(j->second);
                _bottomLayout->setVisible(i->second || j->second);
                i = value.find(WindowControl::Tools);
                _toolWidget->setVisible(i->second);
            });
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
