// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ExportTool.h"

#include "App.h"
#include "FilesModel.h"
#include "PlaybackModel.h"

#include <toucan/Util.h>

#include <dtk/ui/GridLayout.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/Window.h>

namespace toucan
{
    void ExportWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::ExportWidget", parent);

        _host = app->getHost();
        _imageExtensions =
        {
            ".exr",
            ".tiff",
            ".png"
        };
        _movieCodecs =
        {
            "mjpeg"
        };

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        _outputLayout = dtk::VerticalLayout::create(context, _layout);
        _outputLayout->setMarginRole(dtk::SizeRole::Margin);
        auto label = dtk::Label::create(context, "Output directory:", _outputLayout);
        _outputPathEdit = dtk::FileEdit::create(context, _outputLayout);

        auto divider = dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _tabWidget = dtk::TabWidget::create(context, _layout);

        _imageLayout = dtk::VerticalLayout::create(context);
        _imageLayout->setMarginRole(dtk::SizeRole::Margin);
        _tabWidget->addTab("Images", _imageLayout);

        auto gridLayout = dtk::GridLayout::create(context, _imageLayout);

        label = dtk::Label::create(context, "Base name:", gridLayout);
        gridLayout->setGridPos(label, 0, 0);
        _imageBaseNameEdit = dtk::LineEdit::create(context, gridLayout);
        _imageBaseNameEdit->setText("render.");
        gridLayout->setGridPos(_imageBaseNameEdit, 0, 1);

        label = dtk::Label::create(context, "Number padding:", gridLayout);
        gridLayout->setGridPos(label, 1, 0);
        _imagePaddingEdit = dtk::IntEdit::create(context, gridLayout);
        _imagePaddingEdit->setRange(dtk::RangeI(0, 9));
        gridLayout->setGridPos(_imagePaddingEdit, 1, 1);

        label = dtk::Label::create(context, "Extension:", gridLayout);
        gridLayout->setGridPos(label, 2, 0);
        _imageExtensionComboBox = dtk::ComboBox::create(context, _imageExtensions, gridLayout);
        gridLayout->setGridPos(_imageExtensionComboBox, 2, 1);

        divider = dtk::Divider::create(context, dtk::Orientation::Vertical, _imageLayout);

        auto exportSequenceButton = dtk::PushButton::create(
            context,
            "Export Sequence",
            _imageLayout);

        auto exportCurrentButton = dtk::PushButton::create(
            context,
            "Export Current Frame",
            _imageLayout);

        _movieLayout = dtk::VerticalLayout::create(context);
        _movieLayout->setMarginRole(dtk::SizeRole::Margin);
        _tabWidget->addTab("Movie", _movieLayout);

        gridLayout = dtk::GridLayout::create(context, _movieLayout);

        label = dtk::Label::create(context, "Base name:", gridLayout);
        gridLayout->setGridPos(label, 0, 0);
        _movieBaseNameEdit = dtk::LineEdit::create(context, gridLayout);
        _movieBaseNameEdit->setText("render");
        gridLayout->setGridPos(_movieBaseNameEdit, 0, 1);

        label = dtk::Label::create(context, "Codec:", gridLayout);
        gridLayout->setGridPos(label, 1, 0);
        _movieCodecComboBox = dtk::ComboBox::create(context, _movieCodecs, gridLayout);
        gridLayout->setGridPos(_movieCodecComboBox, 1, 1);

        divider = dtk::Divider::create(context, dtk::Orientation::Vertical, _movieLayout);

        auto exportMovieButton = dtk::PushButton::create(
            context,
            "Export Movie",
            _movieLayout);

        exportSequenceButton->setClickedCallback(
            [this]
            {
                _timeRange = _file->getPlaybackModel()->getInOutRange();
                _export();
            });

        exportCurrentButton->setClickedCallback(
            [this]
            {
                _timeRange = OTIO_NS::TimeRange(
                    _file->getPlaybackModel()->getCurrentTime(),
                    OTIO_NS::RationalTime(1.0, _file->getPlaybackModel()->getTimeRange().duration().rate()));
                _export();
            });

        exportMovieButton->setClickedCallback(
            [this]
            {
                _timeRange = _file->getPlaybackModel()->getInOutRange();
                const std::string extension = ".mov";
                const std::filesystem::path path =
                    _outputPathEdit->getPath() /
                    (_movieBaseNameEdit->getText() + extension);
                const IMATH_NAMESPACE::V2d imageSize = _graph->getImageSize();
                _ffWrite = std::make_shared<ffmpeg::Write>(
                    path,
                    OIIO::ImageSpec(imageSize.x, imageSize.y, 3),
                    _timeRange.duration().rate());
                _export();
            });

        _timer = dtk::Timer::create(context);
        _timer->setRepeating(true);

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                if (_file)
                {
                    _graph = std::make_shared<ImageGraph>(
                        _file->getPath(),
                        _file->getTimelineWrapper());
                }
                else
                {
                    _timeRange = OTIO_NS::TimeRange();
                    _time = OTIO_NS::RationalTime();
                    _graph.reset();
                }
                _outputLayout->setEnabled(_file.get());
                _imageLayout->setEnabled(_file.get());
                _movieLayout->setEnabled(_file.get());
            });
    }

    ExportWidget::~ExportWidget()
    {}

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

    void ExportWidget::_export()
    {
        _time = _timeRange.start_time();

        _dialog = dtk::ProgressDialog::create(
            getContext(),
            "Export",
            "Exporting:",
            getWindow());
        _dialog->setCloseCallback(
            [this]
            {
                _timer->stop();
                _ffWrite.reset();
                _dialog.reset();
            });
        _dialog->show();

        _timer->start(
            std::chrono::microseconds(0),
            [this]
            {
                if (auto node = _graph->exec(_host, _time))
                {
                    const auto buf = node->exec();
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
                            _imageExtensions[_imageExtensionComboBox->getCurrentIndex()]);
                        buf.write(fileName);
                    }
                }

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
            });
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
