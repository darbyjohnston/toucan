// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/IToolWidget.h>

#include <toucanRender/FFmpegWrite.h>
#include <toucanRender/ImageGraph.h>

#include <ftk/Ui/ComboBox.h>
#include <ftk/Ui/Divider.h>
#include <ftk/Ui/FileEdit.h>
#include <ftk/Ui/IntEdit.h>
#include <ftk/Ui/Label.h>
#include <ftk/Ui/LineEdit.h>
#include <ftk/Ui/ProgressDialog.h>
#include <ftk/Ui/PushButton.h>
#include <ftk/Ui/RowLayout.h>
#include <ftk/Ui/ScrollWidget.h>
#include <ftk/Ui/Settings.h>
#include <ftk/Ui/TabWidget.h>
#include <ftk/Ui/ToolButton.h>
#include <ftk/Core/Timer.h>

namespace toucan
{
    class File;

    //! Export widget.
    class ExportWidget : public ftk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ExportWidget();

        //! Create a new widget.
        static std::shared_ptr<ExportWidget> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const ftk::Box2I&) override;
        void sizeHintEvent(const ftk::SizeHintEvent&) override;

    private:
        struct SettingsValues
        {
            std::string dir;
            int sizeChoice = 0;
            ftk::Size2I customSize = ftk::Size2I(1920, 1080);
            int currentTab = 0;
            std::string imageBaseName = "render.";
            int imagePadding = 0;
            std::string imageExtension = ".tif";
            std::string movieBaseName = "render";
            std::string movieExtension = ".mov";
            std::string movieCodec = "MJPEG";
        };

        void _initSettings(const std::shared_ptr<ftk::Context>&, SettingsValues&);
        void _initCommonUI(const std::shared_ptr<ftk::Context>&, const SettingsValues&);
        void _initImageUI(const std::shared_ptr<ftk::Context>&, const SettingsValues&);
        void _initMovieUI(const std::shared_ptr<ftk::Context>&, const SettingsValues&);

        enum class ExportType
        {
            Sequence,
            Frame,
            Movie
        };

        void _export(ExportType);
        void _exportFrame();

        void _widgetUpdate();

        std::shared_ptr<ftk::Settings> _settings;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<File> _file;
        OTIO_NS::TimeRange _timeRange;
        OTIO_NS::RationalTime _time;
        std::shared_ptr<ImageGraph> _graph;
        IMATH_NAMESPACE::V2d _imageSize = IMATH_NAMESPACE::V2d(0, 0);
        IMATH_NAMESPACE::V2d _outputSize = IMATH_NAMESPACE::V2d(0, 0);
        std::vector<std::string> _movieCodecs;
        std::shared_ptr<ffmpeg::Write> _ffWrite;

        std::shared_ptr<ftk::VerticalLayout> _layout;
        std::shared_ptr<ftk::FileEdit> _dirEdit;
        std::shared_ptr<ftk::ComboBox> _sizeComboBox;
        std::shared_ptr<ftk::IntEdit> _widthEdit;
        std::shared_ptr<ftk::IntEdit> _heightEdit;
        std::shared_ptr<ftk::TabWidget> _tabWidget;
        std::shared_ptr<ftk::LineEdit> _imageBaseNameEdit;
        std::shared_ptr<ftk::IntEdit> _imagePaddingEdit;
        std::shared_ptr<ftk::LineEdit> _imageExtensionEdit;
        std::shared_ptr<ftk::Label> _imageFilenameLabel;
        std::shared_ptr<ftk::PushButton> _exportSequenceButton;
        std::shared_ptr<ftk::PushButton> _exportFrameButton;
        std::shared_ptr<ftk::LineEdit> _movieBaseNameEdit;
        std::shared_ptr<ftk::LineEdit> _movieExtensionEdit;
        std::shared_ptr<ftk::ComboBox> _movieCodecComboBox;
        std::shared_ptr<ftk::Label> _movieFilenameLabel;
        std::shared_ptr<ftk::PushButton> _exportMovieButton;
        std::shared_ptr<ftk::ProgressDialog> _dialog;

        std::shared_ptr<ftk::Timer> _timer;

        std::shared_ptr<ftk::ValueObserver<std::shared_ptr<File> > > _fileObserver;
    };

    //! Export tool.
    class ExportTool : public IToolWidget
    {
    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~ExportTool();

        //! Create a new tool.
        static std::shared_ptr<ExportTool> create(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const ftk::Box2I&) override;
        void sizeHintEvent(const ftk::SizeHintEvent&) override;

    private:
        std::shared_ptr<ftk::ScrollWidget> _scrollWidget;
        std::shared_ptr<ExportWidget> _widget;
    };
}

