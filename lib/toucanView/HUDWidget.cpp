// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "HUDWidget.h"

#include "App.h"
#include "FilesModel.h"

#include <dtk/ui/Spacer.h>
#include <dtk/core/Format.h>

namespace toucan
{
    void HUDWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<File>& file,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::HUDWidget", parent);

        _file = file;

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setMarginRole(dtk::SizeRole::MarginSmall);
        _layout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        auto hLayout = dtk::HorizontalLayout::create(context, _layout);
        hLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        _labels["Path"] = dtk::Label::create(
            context,
            file->getPath().filename().string(),
            hLayout);
        _labels["Path"]->setFontRole(dtk::FontRole::Mono);
        _labels["Path"]->setMarginRole(dtk::SizeRole::MarginInside);
        _labels["Path"]->setBackgroundRole(dtk::ColorRole::Overlay);

        auto spacer = dtk::Spacer::create(context, dtk::Orientation::Horizontal, hLayout);
        spacer->setHStretch(dtk::Stretch::Expanding);

        const IMATH_NAMESPACE::V2i& imageSize = file->getImageSize();
        _labels["Image"] = dtk::Label::create(
            context,
            dtk::Format("{0}x{1}x{2} {3}").
            arg(imageSize.x).
            arg(imageSize.y).
            arg(file->getImageChannels()).
            arg(file->getImageDataType()),
            hLayout);
        _labels["Image"]->setFontRole(dtk::FontRole::Mono);
        _labels["Image"]->setMarginRole(dtk::SizeRole::MarginInside);
        _labels["Image"]->setBackgroundRole(dtk::ColorRole::Overlay);

        spacer = dtk::Spacer::create(context, dtk::Orientation::Vertical, _layout);
        spacer->setVStretch(dtk::Stretch::Expanding);

        hLayout = dtk::HorizontalLayout::create(context, _layout);
        hLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        spacer = dtk::Spacer::create(context, dtk::Orientation::Horizontal, hLayout);
        spacer->setHStretch(dtk::Stretch::Expanding);

        _labels["Time"] = dtk::Label::create(context, hLayout);
        _labels["Time"]->setFontRole(dtk::FontRole::Mono);
        _labels["Time"]->setMarginRole(dtk::SizeRole::MarginInside);
        _labels["Time"]->setBackgroundRole(dtk::ColorRole::Overlay);

        _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
            file->getPlaybackModel()->observeCurrentTime(),
            [this](const OTIO_NS::RationalTime& value)
            {
                _currentTime = value;
                _widgetUpdate();
            });

        _timeRangeObserver = dtk::ValueObserver<OTIO_NS::TimeRange>::create(
            file->getPlaybackModel()->observeTimeRange(),
            [this](const OTIO_NS::TimeRange& value)
            {
                _timeRange = value;
                _widgetUpdate();
            });
    }

    HUDWidget::~HUDWidget()
    {}

    std::shared_ptr<HUDWidget> HUDWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<File>& file,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<HUDWidget>(new HUDWidget);
        out->_init(context, app, file, parent);
        return out;
    }

    void HUDWidget::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void HUDWidget::_widgetUpdate()
    {
        _labels["Time"]->setText(
            dtk::Format("{0} / {1} - {2} @ {4}").
            arg(_currentTime.value()).
            arg(_timeRange.start_time().value()).
            arg(_timeRange.end_time_inclusive().value()).
            arg(_timeRange.duration().rate()));
    }
}
