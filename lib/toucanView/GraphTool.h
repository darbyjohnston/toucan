// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "IToolWidget.h"

#include <toucan/ImageGraph.h>

#include <dtk/ui/ButtonGroup.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>

namespace toucan
{
    class File;

    class GraphWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~GraphWidget();

        static std::shared_ptr<GraphWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void drawEvent(const dtk::Box2I&, const dtk::DrawEvent&) override;

    private:
        int _getDepth(const std::shared_ptr<IImageNode>&, int = 0) const;

        void _createNodes(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IImageNode>&,
            int = 0);

        void _drawInputs(
            const std::shared_ptr<IImageNode>&,
            const dtk::Box2I& drawRect,
            const dtk::DrawEvent&,
            const dtk::LineOptions&);

        void _graphUpdate();

        std::shared_ptr<File> _file;
        std::shared_ptr<IImageNode> _rootNode;
        int _depth = 0;
        std::shared_ptr<IImageNode> _currentNode;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::vector<std::shared_ptr<dtk::HorizontalLayout> > _layouts;
        std::shared_ptr<dtk::ButtonGroup> _buttonGroup;
        std::vector<std::shared_ptr<dtk::PushButton> > _buttons;
        std::map<std::shared_ptr<IImageNode>, std::shared_ptr<dtk::PushButton> > _nodeToButton;
        std::map<std::shared_ptr<dtk::PushButton>, std::shared_ptr<IImageNode> > _buttonToNode;

        struct SizeData
        {
            bool init = true;
            float displayScale = 0.F;
            int lineWidth = 0;
        };
        SizeData _size;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<IImageNode> > > _rootNodeObserver;
        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<IImageNode> > > _currentNodeObserver;
    };

    class GraphTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~GraphTool();

        static std::shared_ptr<GraphTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<GraphWidget> _widget;
    };
}

