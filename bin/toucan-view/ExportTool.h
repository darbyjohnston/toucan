// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "IToolWidget.h"

#include <dtk/ui/RowLayout.h>

namespace toucan
{
    class ExportWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ExportWidget();

        static std::shared_ptr<ExportWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::VerticalLayout> _layout;
    };

    class ExportTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ExportTool();

        static std::shared_ptr<ExportTool> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<ExportWidget> _widget;
    };
}

