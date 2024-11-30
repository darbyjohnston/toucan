// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "IToolWidget.h"

#include <toucan/ImageGraph.h>

#include <dtk/ui/ComboBox.h>
#include <dtk/ui/Divider.h>
#include <dtk/ui/FileEdit.h>
#include <dtk/ui/IntEdit.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/LineEdit.h>
#include <dtk/ui/ProgressDialog.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/core/Timer.h>

namespace toucan
{
    class File;

    class ExportWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ExportWidget();

        static std::shared_ptr<ExportWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _export();

        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<File> _file;
        OTIO_NS::TimeRange _timeRange;
        OTIO_NS::RationalTime _time;
        std::shared_ptr<ImageGraph> _graph;
        std::vector<std::string> _formats;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::FileEdit> _outputPathEdit;
        std::shared_ptr<dtk::LineEdit> _baseNameEdit;
        std::shared_ptr<dtk::IntEdit> _paddingEdit;
        std::shared_ptr<dtk::ComboBox> _formatComboBox;
        std::shared_ptr<dtk::ProgressDialog> _dialog;
        std::shared_ptr<dtk::Timer> _timer;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
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
        std::shared_ptr<dtk::ScrollWidget> _scrollWidget;
        std::shared_ptr<ExportWidget> _widget;
    };
}

