// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "GraphTool.h"

#include "App.h"
#include "Document.h"

#include <dtk/ui/Spacer.h>

namespace toucan
{
    void GraphWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::GraphWidget", parent);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setMarginRole(dtk::SizeRole::MarginLarge);
        _layout->setSpacingRole(dtk::SizeRole::SpacingLarge);

        _buttonGroup = dtk::ButtonGroup::create(context, dtk::ButtonGroupType::Radio);

        _buttonGroup->setCheckedCallback(
            [this](int index, bool value)
            {
                if (_document && index >= 0 && index < _buttons.size())
                {
                    auto i = _buttonToNode.find(_buttons[index]);
                    if (i != _buttonToNode.end())
                    {
                        _document->setCurrentNode(i->second);
                    }
                }
            });

        _documentObserver = dtk::ValueObserver<std::shared_ptr<Document> >::create(
            app->getDocumentsModel()->observeCurrent(),
            [this](const std::shared_ptr<Document>& document)
            {
                _document = document;
                if (document)
                {
                    _rootNodeObserver = dtk::ValueObserver<std::shared_ptr<IImageNode> >::create(
                        document->observeRootNode(),
                        [this](const std::shared_ptr<IImageNode>& node)
                        {
                            _rootNode = node;
                            _depth = _getDepth(_rootNode);
                            _graphUpdate();
                        });

                    _currentNodeObserver = dtk::ValueObserver<std::shared_ptr<IImageNode> >::create(
                        document->observeCurrentNode(),
                        [this](const std::shared_ptr<IImageNode>& node)
                        {
                            _currentNode = node;
                            auto i = _nodeToButton.find(node);
                            if (i != _nodeToButton.end())
                            {
                                i->second->setChecked(true);
                            }
                        });
                }
                else
                {
                    _rootNode.reset();
                    _depth = 0;
                    _currentNode.reset();
                    _rootNodeObserver.reset();
                    _currentNodeObserver.reset();
                    _graphUpdate();
                }
            });
    }

    GraphWidget::~GraphWidget()
    {}

    std::shared_ptr<GraphWidget> GraphWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<GraphWidget>(new GraphWidget);
        out->_init(context, app, parent);
        return out;
    }

    void GraphWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void GraphWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.lineWidth = event.style->getSizeRole(dtk::SizeRole::BorderFocus, event.displayScale);
        }
        _setSizeHint(_layout->getSizeHint());
    }

    void GraphWidget::drawEvent(const dtk::Box2I& drawRect, const dtk::DrawEvent& event)
    {
        IWidget::drawEvent(drawRect, event);
        if (_rootNode)
        {
            dtk::LineOptions options;
            options.width = _size.lineWidth;
            _drawInputs(_rootNode, drawRect, event, options);
        }
    }

    int GraphWidget::_getDepth(const std::shared_ptr<IImageNode>& node, int depth) const
    {
        int out = depth + 1;
        for (const auto& input : node->getInputs())
        {
            out = std::max(out, _getDepth(input, out));
        }
        return out;
    }

    void GraphWidget::_createNodes(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<IImageNode>& node,
        int depth)
    {
        auto button = dtk::PushButton::create(context, node->getLabel(), _layouts[_depth - 1 - depth]);
        _buttonGroup->addButton(button);
        _buttons.push_back(button);
        _nodeToButton[node] = button;
        _buttonToNode[button] = node;
        for (const auto& input : node->getInputs())
        {
            _createNodes(context, input, depth + 1);
        }
    }

    void GraphWidget::_drawInputs(
        const std::shared_ptr<IImageNode>& node,
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event,
        const dtk::LineOptions& options)
    {
        auto i = _nodeToButton.find(node);
        if (i != _nodeToButton.end())
        {
            dtk::Box2I g = i->second->getGeometry();
            const dtk::V2I start = dtk::center(g);
            std::vector<std::pair<dtk::V2I, dtk::V2I> > lines;
            for (const auto& input : node->getInputs())
            {
                auto j = _nodeToButton.find(input);
                if (j != _nodeToButton.end())
                {
                    g = j->second->getGeometry();
                    const dtk::V2I end = dtk::center(g);
                    const dtk::V2I v0(start.x, (start.y + end.y) / 2);
                    const dtk::V2I v1(end.x, (start.y + end.y) / 2);
                    lines.push_back(std::make_pair(start, v0));
                    lines.push_back(std::make_pair(v0, v1));
                    lines.push_back(std::make_pair(v1, end));
                }
            }
            event.render->drawLines(
                lines,
                event.style->getColorRole(dtk::ColorRole::Checked),
                options);
            for (const auto& input : node->getInputs())
            {
                _drawInputs(input, drawRect, event, options);
            }
        }
    }

    void GraphWidget::_graphUpdate()
    {
        _buttonGroup->clearButtons();
        _buttons.clear();
        _nodeToButton.clear();
        _buttonToNode.clear();
        for (const auto& layout : _layouts)
        {
            layout->setParent(nullptr);
        }
        _layouts.clear();
        if (_rootNode)
        {
            auto context = getContext();
            for (int i = 0; i < _depth; ++i)
            {
                auto layout = dtk::HorizontalLayout::create(context, _layout);
                auto spacer = dtk::Spacer::create(context, dtk::Orientation::Horizontal, layout);
                spacer->setSpacingRole(dtk::SizeRole::None);
                spacer->setStretch(dtk::Stretch::Expanding);
                _layouts.push_back(layout);
            }
            _createNodes(context, _rootNode);
            for (const auto& layout : _layouts)
            {
                auto spacer = dtk::Spacer::create(context, dtk::Orientation::Horizontal, layout);
                spacer->setSpacingRole(dtk::SizeRole::None);
                spacer->setStretch(dtk::Stretch::Expanding);
            }
        }
    }

    void GraphTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::GraphTool", "Graph", parent);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, shared_from_this());
        _scrollWidget->setBorder(false);

        _widget = GraphWidget::create(context, app);
        _scrollWidget->setWidget(_widget);
    }

    GraphTool::~GraphTool()
    {}

    std::shared_ptr<GraphTool> GraphTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<GraphTool>(new GraphTool);
        out->_init(context, app, parent);
        return out;
    }

    void GraphTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _scrollWidget->setGeometry(value);
    }

    void GraphTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_scrollWidget->getSizeHint());
    }
}
