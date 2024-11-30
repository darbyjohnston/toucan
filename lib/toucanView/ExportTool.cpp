// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ExportTool.h"

#include "App.h"
#include "DocumentsModel.h"
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
        _formats =
        {
            ".tiff",
            ".png"
        };

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setMarginRole(dtk::SizeRole::MarginSmall);
        _layout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        auto vLayout = dtk::VerticalLayout::create(context, _layout);
        vLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
        auto label = dtk::Label::create(context, "Output directory:", vLayout);
        _outputPathEdit = dtk::FileEdit::create(context, vLayout);

        auto gridLayout = dtk::GridLayout::create(context, _layout);
        gridLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        label = dtk::Label::create(context, "Base name:", gridLayout);
        gridLayout->setGridPos(label, 0, 0);
        _baseNameEdit = dtk::LineEdit::create(context, gridLayout);
        _baseNameEdit->setText("render.");
        gridLayout->setGridPos(_baseNameEdit, 0, 1);

        label = dtk::Label::create(context, "Number padding:", gridLayout);
        gridLayout->setGridPos(label, 1, 0);
        _paddingEdit = dtk::IntEdit::create(context, gridLayout);
        _paddingEdit->setRange(dtk::RangeI(0, 9));
        gridLayout->setGridPos(_paddingEdit, 1, 1);

        label = dtk::Label::create(context, "File format:", gridLayout);
        gridLayout->setGridPos(label, 2, 0);
        _formatComboBox = dtk::ComboBox::create(context, _formats, gridLayout);
        gridLayout->setGridPos(_formatComboBox, 2, 1);

        auto divider = dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        auto exportSequenceButton = dtk::PushButton::create(
            context,
            "Export Sequence",
            _layout);
        exportSequenceButton->setClickedCallback(
            [this]
            {
                _timeRange = _document->getPlaybackModel()->getInOutRange();
                _export();
            });

        auto exportCurrentButton = dtk::PushButton::create(
            context,
            "Export Current Frame",
            _layout);
        exportCurrentButton->setClickedCallback(
            [this]
            {
                _timeRange = OTIO_NS::TimeRange(
                    _document->getPlaybackModel()->getCurrentTime(),
                    OTIO_NS::RationalTime(1.0, _document->getPlaybackModel()->getTimeRange().duration().rate()));
                _export();
            });

        _timer = dtk::Timer::create(context);
        _timer->setRepeating(true);

        _documentObserver = dtk::ValueObserver<std::shared_ptr<Document> >::create(
            app->getDocumentsModel()->observeCurrent(),
            [this](const std::shared_ptr<Document>& document)
            {
                _document = document;
                if (_document)
                {
                    _graph = std::make_shared<ImageGraph>(
                        _document->getPath(),
                        _document->getTimelineWrapper());
                }
                else
                {
                    _timeRange = OTIO_NS::TimeRange();
                    _time = OTIO_NS::RationalTime();
                    _graph.reset();
                }
                setEnabled(_document.get());
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
                    const std::string fileName = getSequenceFrame(
                        _outputPathEdit->getPath().string(),
                        _baseNameEdit->getText(),
                        _time.to_frames(),
                        _paddingEdit->getValue(),
                        _formats[_formatComboBox->getCurrentIndex()]);
                    buf.write(fileName);
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
