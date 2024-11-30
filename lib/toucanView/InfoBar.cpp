// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "InfoBar.h"

#include "App.h"
#include "FilesModel.h"

#include <dtk/core/Format.h>
#include <dtk/core/String.h>

namespace toucan
{
    void InfoBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::InfoBar", parent);

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _label = dtk::Label::create(context, _layout);
        _label->setMarginRole(dtk::SizeRole::MarginInside);

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                std::string text;
                std::string tooltip;
                if (file)
                {
                    const IMATH_NAMESPACE::V2i& imageSize = file->getImageSize();
                    const size_t trackCount = file->getTimeline()->find_children<OTIO_NS::Track>().size();

                    text = dtk::Format("{0}: {1}x{2}, {3} image channels, {4} pixel data, {5} tracks").
                        arg(dtk::elide(file->getPath().filename().string())).
                        arg(imageSize.x).
                        arg(imageSize.y).
                        arg(file->getImageChannels()).
                        arg(file->getImageDataType()).
                        arg(trackCount);
                    tooltip = dtk::Format(
                        "Path: {0}\n"
                        "Render: {1}x{2}, {3} image channels, {4} pixel data\n"
                        "Tracks: {5}").
                        arg(file->getPath().string()).
                        arg(imageSize.x).
                        arg(imageSize.y).
                        arg(file->getImageChannels()).
                        arg(file->getImageDataType()).
                        arg(trackCount);
                }
                _label->setText(text);
                _label->setTooltip(tooltip);
            });
    }

    InfoBar::~InfoBar()
    {}

    std::shared_ptr<InfoBar> InfoBar::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<InfoBar>(new InfoBar);
        out->_init(context, app, parent);
        return out;
    }

    void InfoBar::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void InfoBar::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }
}
