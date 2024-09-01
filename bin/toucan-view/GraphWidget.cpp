// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "GraphWidget.h"

using namespace dtk;
using namespace dtk::core;
using namespace dtk::ui;

namespace toucan
{
    void GraphWidget::_init(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::GraphWidget", parent);
    }

    GraphWidget::~GraphWidget()
    {}

    std::shared_ptr<GraphWidget> GraphWidget::create(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<GraphWidget>(new GraphWidget);
        out->_init(context, parent);
        return out;
    }
}
