// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ExportTool.h"

#include "App.h"
#include "FilesModel.h"
#include "PlaybackModel.h"

#include <toucanRender/Util.h>

#include <dtk/ui/DialogSystem.h>
#include <dtk/ui/GridLayout.h>
#include <dtk/ui/Settings.h>
#include <dtk/ui/Window.h>
#include <dtk/core/Context.h>

#include <nlohmann/json.hpp>

#include <sstream>

namespace toucan
{
    void ExportWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::ExportWidget", parent);

        _host = app->getHost();
        _movieCodecs = ffmpeg::getVideoCodecStrings();

        std::string outputPath;
        int currentTab = 0;
        std::string imageBaseName = "render.";
        int imagePadding = 0;
        std::string imageExtension = ".tiff";
        std::string movieBaseName = "render";
        std::string movieExtension = ".mov";
        std::string movieCodec = "MJPEG";
        try
        {
            auto settings = context->getSystem<dtk::Settings>();
            const auto json = std::any_cast<nlohmann::json>(settings->get("ExportWidget"));
            auto i = json.find("OutputPath");
            if (i != json.end() && i->is_string())
            {
                outputPath = i->get<std::string>();
            }
            i = json.find("CurrentTab");
            if (i != json.end() && i->is_number())
            {
                currentTab = i->get<int>();
            }
            i = json.find("ImageBaseName");
            if (i != json.end() && i->is_string())
            {
                imageBaseName = i->get<std::string>();
            }
            i = json.find("ImagePadding");
            if (i != json.end() && i->is_number())
            {
                imagePadding = i->get<int>();
            }
            i = json.find("ImageExtension");
            if (i != json.end() && i->is_string())
            {
                imageExtension = i->get<std::string>();
            }
            i = json.find("MovieBaseName");
            if (i != json.end() && i->is_string())
            {
                movieBaseName = i->get<std::string>();
            }
            i = json.find("MovieExtension");
            if (i != json.end() && i->is_string())
            {
                movieExtension = i->get<std::string>();
            }
            i = json.find("MovieCodec");
            if (i != json.end() && i->is_string())
            {
                movieCodec = i->get<std::string>();
            }
        }
        catch (const std::exception&)
        {}

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        _outputLayout = dtk::VerticalLayout::create(context, _layout);
        _outputLayout->setMarginRole(dtk::SizeRole::Margin);
        auto label = dtk::Label::create(context, "Output directory:", _outputLayout);
        _outputPathEdit = dtk::FileEdit::create(context, _outputLayout);
        _outputPathEdit->setPath(outputPath);

        auto divider = dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _tabWidget = dtk::TabWidget::create(context, _layout);

        _imageLayout = dtk::VerticalLayout::create(context);
        _imageLayout->setMarginRole(dtk::SizeRole::Margin);
        _tabWidget->addTab("Images", _imageLayout);

        auto gridLayout = dtk::GridLayout::create(context, _imageLayout);

        label = dtk::Label::create(context, "Base name:", gridLayout);
        gridLayout->setGridPos(label, 0, 0);
        _imageBaseNameEdit = dtk::LineEdit::create(context, gridLayout);
        _imageBaseNameEdit->setText(imageBaseName);
        gridLayout->setGridPos(_imageBaseNameEdit, 0, 1);

        label = dtk::Label::create(context, "Number padding:", gridLayout);
        gridLayout->setGridPos(label, 1, 0);
        _imagePaddingEdit = dtk::IntEdit::create(context, gridLayout);
        _imagePaddingEdit->setRange(dtk::RangeI(0, 9));
        _imagePaddingEdit->setValue(imagePadding);
        gridLayout->setGridPos(_imagePaddingEdit, 1, 1);

        label = dtk::Label::create(context, "Extension:", gridLayout);
        gridLayout->setGridPos(label, 2, 0);
        _imageExtensionEdit = dtk::LineEdit::create(context, gridLayout);
        _imageExtensionEdit->setText(imageExtension);
        gridLayout->setGridPos(_imageExtensionEdit, 2, 1);

        label = dtk::Label::create(context, "Filename:", gridLayout);
        gridLayout->setGridPos(label, 3, 0);
        _imageFilenameLabel = dtk::Label::create(context, gridLayout);
        gridLayout->setGridPos(_imageFilenameLabel, 3, 1);

        divider = dtk::Divider::create(context, dtk::Orientation::Vertical, _imageLayout);

        _exportSequenceButton = dtk::PushButton::create(
            context,
            "Export Sequence",
            _imageLayout);

        _exportStillButton = dtk::PushButton::create(
            context,
            "Export Still Frame",
            _imageLayout);

        _movieLayout = dtk::VerticalLayout::create(context);
        _movieLayout->setMarginRole(dtk::SizeRole::Margin);
        _tabWidget->addTab("Movie", _movieLayout);
        _tabWidget->setCurrentTab(currentTab);

        gridLayout = dtk::GridLayout::create(context, _movieLayout);

        label = dtk::Label::create(context, "Base name:", gridLayout);
        gridLayout->setGridPos(label, 0, 0);
        _movieBaseNameEdit = dtk::LineEdit::create(context, gridLayout);
        _movieBaseNameEdit->setText(movieBaseName);
        gridLayout->setGridPos(_movieBaseNameEdit, 0, 1);

        label = dtk::Label::create(context, "Extension:", gridLayout);
        gridLayout->setGridPos(label, 1, 0);
        _movieExtensionEdit = dtk::LineEdit::create(context, gridLayout);
        _movieExtensionEdit->setText(movieExtension);
        gridLayout->setGridPos(_movieExtensionEdit, 1, 1);

        label = dtk::Label::create(context, "Filename:", gridLayout);
        gridLayout->setGridPos(label, 2, 0);
        _movieFilenameLabel = dtk::Label::create(context, gridLayout);
        gridLayout->setGridPos(_movieFilenameLabel, 2, 1);

        label = dtk::Label::create(context, "Codec:", gridLayout);
        gridLayout->setGridPos(label, 3, 0);
        _movieCodecComboBox = dtk::ComboBox::create(context, _movieCodecs, gridLayout);
        ffmpeg::VideoCodec ffmpegVideoCodec = ffmpeg::VideoCodec::First;
        ffmpeg::fromString(movieCodec, ffmpegVideoCodec);
        _movieCodecComboBox->setCurrentIndex(static_cast<int>(ffmpegVideoCodec));
        gridLayout->setGridPos(_movieCodecComboBox, 3, 1);

        divider = dtk::Divider::create(context, dtk::Orientation::Vertical, _movieLayout);

        _exportMovieButton = dtk::PushButton::create(
            context,
            "Export Movie",
            _movieLayout);
        
        _widgetUpdate();

        _imageBaseNameEdit->setTextChangedCallback(
            [this](const std::string&)
            {
                _widgetUpdate();
            });

        _imagePaddingEdit->setCallback(
            [this](int)
            {
                _widgetUpdate();
            });

        _imageExtensionEdit->setTextChangedCallback(
            [this](const std::string&)
            {
                _widgetUpdate();
            });

        _movieBaseNameEdit->setTextChangedCallback(
            [this](const std::string&)
            {
                _widgetUpdate();
            });

        _movieExtensionEdit->setTextChangedCallback(
            [this](const std::string&)
            {
                _widgetUpdate();
            });

        _exportSequenceButton->setClickedCallback(
            [this]
            {
                _export(ExportType::Sequence);
            });

        _exportStillButton->setClickedCallback(
            [this]
            {
                _export(ExportType::Still);
            });

        _exportMovieButton->setClickedCallback(
            [this]
            {
                _export(ExportType::Movie);
            });

        _timer = dtk::Timer::create(context);
        _timer->setRepeating(true);

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                _exportSequenceButton->setEnabled(_file.get());
                _exportStillButton->setEnabled(_file.get());
                _exportMovieButton->setEnabled(_file.get());
            });
    }

    ExportWidget::~ExportWidget()
    {
        nlohmann::json json;
        json["OutputPath"] = _outputPathEdit->getPath().string();
        json["CurrentTab"] = _tabWidget->getCurrentTab();
        json["ImageBaseName"] = _imageBaseNameEdit->getText();
        json["ImagePadding"] = _imagePaddingEdit->getValue();
        json["ImageExtension"] = _imageExtensionEdit->getText();
        json["MovieBaseName"] = _movieBaseNameEdit->getText();
        json["MovieExtension"] = _movieExtensionEdit->getText();
        json["MovieCodec"] = ffmpeg::toString(
            static_cast<ffmpeg::VideoCodec>(_movieCodecComboBox->getCurrentIndex()));
        auto context = getContext();
        auto settings = context->getSystem<dtk::Settings>();
        settings->set("ExportWidget", json);
    }

    std::shared_ptr<ExportWidget> ExportWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<ExportWidget>(new ExportWidget);
        out->_init(context, app, parent);
        return out;
    }

    void ExportWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void ExportWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void ExportWidget::_export(ExportType type)
    {
        if (!_file)
            return;

        _graph = std::make_shared<ImageGraph>(
            getContext(),
            _file->getPath(),
            _file->getTimelineWrapper());
        _imageSize = _graph->getImageSize();

        switch (type)
        {
        case ExportType::Sequence:
            _timeRange = _file->getPlaybackModel()->getInOutRange();
            break;
        case ExportType::Still:
            _timeRange = OTIO_NS::TimeRange(
                _file->getPlaybackModel()->getCurrentTime(),
                OTIO_NS::RationalTime(1.0, _file->getPlaybackModel()->getTimeRange().duration().rate()));
            break;
        case ExportType::Movie:
        {
            _timeRange = _file->getPlaybackModel()->getInOutRange();
            const std::string baseName = _movieBaseNameEdit->getText();
            const std::string extension = _movieExtensionEdit->getText();
            const std::filesystem::path path = _outputPathEdit->getPath() / (baseName + extension);
            ffmpeg::VideoCodec videoCodec = ffmpeg::VideoCodec::First;
            ffmpeg::fromString(_movieCodecs[_movieCodecComboBox->getCurrentIndex()], videoCodec);
            try
            {
                _ffWrite = std::make_shared<ffmpeg::Write>(
                    path,
                    OIIO::ImageSpec(_imageSize.x, _imageSize.y, 3),
                    _timeRange,
                    videoCodec);
            }
            catch (const std::exception& e)
            {
                auto dialogSystem = getContext()->getSystem<dtk::DialogSystem>();
                dialogSystem->message("ERROR", e.what(), getWindow());
            }
            break;
        }
        default: break;
        }

        _dialog = dtk::ProgressDialog::create(
            getContext(),
            "Export",
            "Exporting:",
            getWindow());
        _dialog->setCloseCallback(
            [this]
            {
                _timer->stop();
                _graph.reset();
                _ffWrite.reset();
                _dialog.reset();
            });
        _dialog->show();

        _time = _timeRange.start_time();
        _timer->start(
            std::chrono::microseconds(0),
            [this]
            {
                _exportFrame();
            });
    }

    void ExportWidget::_exportFrame()
    {
        if (auto node = _graph->exec(_host, _time))
        {
            const auto buf = node->exec();
            try
            {
                if (_ffWrite)
                {
                    _ffWrite->writeImage(buf, _time);
                }
                else
                {
                    const std::string fileName = getSequenceFrame(
                        _outputPathEdit->getPath().string(),
                        _imageBaseNameEdit->getText(),
                        _time.to_frames(),
                        _imagePaddingEdit->getValue(),
                        _imageExtensionEdit->getText());
                    buf.write(fileName);
                }
            }
            catch (const std::exception& e)
            {
                if (_dialog)
                {
                    _dialog->close();
                }
                getContext()->getSystem<dtk::DialogSystem>()->message(
                    "ERROR",
                    e.what(),
                    getWindow());
            }
        }

        if (_dialog)
        {
            const OTIO_NS::RationalTime end = _timeRange.end_time_inclusive();
            if (_time < end)
            {
                _time += OTIO_NS::RationalTime(1.0, _timeRange.duration().rate());
                const OTIO_NS::RationalTime duration = _timeRange.duration();
                const double v = duration.value() > 0.0 ?
                    (_time - _timeRange.start_time()).value() / static_cast<double>(duration.value()) :
                    0.0;
                _dialog->setValue(v);
            }
            else
            {
                _dialog->close();
            }
        }
    }

    void ExportWidget::_widgetUpdate()
    {
        _imageFilenameLabel->setText(getSequenceFrame(
            _outputPathEdit->getPath().string(),
            _imageBaseNameEdit->getText(),
            0,
            _imagePaddingEdit->getValue(),
            _imageExtensionEdit->getText()));

        _movieFilenameLabel->setText(
            _movieBaseNameEdit->getText() +
            _movieExtensionEdit->getText());
    }

    void ExportTool::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IToolWidget::_init(context, app, "toucan::ExportTool", "Export", parent);

        _scrollWidget = dtk::ScrollWidget::create(context, dtk::ScrollType::Both, shared_from_this());
        _scrollWidget->setBorder(false);

        _widget = ExportWidget::create(context, app);
        _scrollWidget->setWidget(_widget);
    }

    ExportTool::~ExportTool()
    {}

    std::shared_ptr<ExportTool> ExportTool::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<ExportTool>(new ExportTool);
        out->_init(context, app, parent);
        return out;
    }

    void ExportTool::setGeometry(const dtk::Box2I& value)
    {
        IToolWidget::setGeometry(value);
        _scrollWidget->setGeometry(value);
    }

    void ExportTool::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IToolWidget::sizeHintEvent(event);
        _setSizeHint(_scrollWidget->getSizeHint());
    }
}
