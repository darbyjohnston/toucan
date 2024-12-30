// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/IToolWidget.h>

#include <toucanRender/FFmpegWrite.h>
#include <toucanRender/ImageGraph.h>

#include <dtk/ui/ComboBox.h>
#include <dtk/ui/Divider.h>
#include <dtk/ui/FileEdit.h>
#include <dtk/ui/IntEdit.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/LineEdit.h>
#include <dtk/ui/ProgressDialog.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/ui/TabWidget.h>
#include <dtk/ui/ToolButton.h>
#include <dtk/core/Timer.h>

namespace toucan
{
    class File;

    //! Export widget.
    class ExportWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ExportWidget();

        //! Create a new widget.
        static std::shared_ptr<ExportWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        struct SettingsValues
        {
            std::string dir;
            int sizeChoice = 0;
            dtk::Size2I customSize = dtk::Size2I(1920, 1080);
            int currentTab = 0;
            std::string imageBaseName = "render.";
            int imagePadding = 0;
            std::string imageExtension = ".tif";
            std::string movieBaseName = "render";
            std::string movieExtension = ".mov";
            std::string movieCodec = "MJPEG";
        };

        void _initSettings(const std::shared_ptr<dtk::Context>&, SettingsValues&);
        void _initCommonUI(const std::shared_ptr<dtk::Context>&, const SettingsValues&);
        void _initImageUI(const std::shared_ptr<dtk::Context>&, const SettingsValues&);
        void _initMovieUI(const std::shared_ptr<dtk::Context>&, const SettingsValues&);

        enum class ExportType
        {
            Sequence,
            Frame,
            Movie
        };

        void _export(ExportType);
        void _exportFrame();

        void _widgetUpdate();

        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<File> _file;
        OTIO_NS::TimeRange _timeRange;
        OTIO_NS::RationalTime _time;
        std::shared_ptr<ImageGraph> _graph;
        IMATH_NAMESPACE::V2d _imageSize = IMATH_NAMESPACE::V2d(0, 0);
        IMATH_NAMESPACE::V2d _outputSize = IMATH_NAMESPACE::V2d(0, 0);
        std::vector<std::string> _movieCodecs;
        std::shared_ptr<ffmpeg::Write> _ffWrite;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::FileEdit> _dirEdit;
        std::shared_ptr<dtk::ComboBox> _sizeComboBox;
        std::shared_ptr<dtk::IntEdit> _widthEdit;
        std::shared_ptr<dtk::IntEdit> _heightEdit;
        std::shared_ptr<dtk::TabWidget> _tabWidget;
        std::shared_ptr<dtk::LineEdit> _imageBaseNameEdit;
        std::shared_ptr<dtk::IntEdit> _imagePaddingEdit;
        std::shared_ptr<dtk::LineEdit> _imageExtensionEdit;
        std::shared_ptr<dtk::Label> _imageFilenameLabel;
        std::shared_ptr<dtk::PushButton> _exportSequenceButton;
        std::shared_ptr<dtk::PushButton> _exportFrameButton;
        std::shared_ptr<dtk::LineEdit> _movieBaseNameEdit;
        std::shared_ptr<dtk::LineEdit> _movieExtensionEdit;
        std::shared_ptr<dtk::ComboBox> _movieCodecComboBox;
        std::shared_ptr<dtk::Label> _movieFilenameLabel;
        std::shared_ptr<dtk::PushButton> _exportMovieButton;
        std::shared_ptr<dtk::ProgressDialog> _dialog;

        std::shared_ptr<dtk::Timer> _timer;

        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
    };

    //! Export tool.
    class ExportTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ExportTool();

        //! Create a new tool.
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

