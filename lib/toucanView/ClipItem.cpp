// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ClipItem.h"

#include "App.h"
#include "File.h"

#include <toucanRender/Util.h>

#include <dtk/ui/DrawUtil.h>

#include <opentimelineio/externalReference.h>
#include <opentimelineio/imageSequenceReference.h>

namespace toucan
{
    void ClipItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<File>& file,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& clip,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const dtk::Color4F& color,
        const std::shared_ptr<IWidget>& parent)
    {
        OTIO_NS::TimeRange timeRange = clip->transformed_time_range(
            clip->trimmed_range(),
            timeline->tracks());
        if (timeline->global_start_time().has_value())
        {
            timeRange = OTIO_NS::TimeRange(
                timeline->global_start_time().value() + timeRange.start_time(),
                timeRange.duration());
        }
        timeRange = OTIO_NS::TimeRange(
            timeRange.start_time().round(),
            timeRange.duration().round());
        IItem::_init(
            context,
            app,
            file,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::SerializableObjectWithMetadata>(clip),
            timeRange,
            "toucan::ClipItem",
            parent);

        _clip = clip;
        _text = !clip->name().empty() ? clip->name() : "Clip";
        _color = color;

        setTooltip(clip->schema_name() + ": " + _text);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _label = ItemLabel::create(context, _layout);
        _label->setName(_text);

        const auto& markers = clip->markers();
        if (!markers.empty())
        {
            _markerLayout = TimeLayout::create(context, timeRange, _layout);
            for (const auto& marker : markers)
            {
                OTIO_NS::TimeRange markerTimeRange = clip->transformed_time_range(
                    marker->marked_range(),
                    timeline->tracks());
                if (timeline->global_start_time().has_value())
                {
                    markerTimeRange = OTIO_NS::TimeRange(
                        timeline->global_start_time().value() + markerTimeRange.start_time(),
                        markerTimeRange.duration());
                }
                auto markerItem = MarkerItem::create(
                    context,
                    app,
                    file,
                    marker,
                    markerTimeRange,
                    _markerLayout);
                _markerItems.push_back(markerItem);
            }
        }

        _textUpdate();
    }
    
    ClipItem::~ClipItem()
    {}

    std::shared_ptr<ClipItem> ClipItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<File>& file,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& clip,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const dtk::Color4F& color,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<ClipItem>();
        out->_init(context, app, file, clip, timeline, color, parent);
        return out;
    }

    void ClipItem::setScale(double value)
    {
        IItem::setScale(value);
        if (_markerLayout)
        {
            _markerLayout->setScale(value);
        }
    }

    void ClipItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        _layout->setGeometry(value);
        _geom.g2 = dtk::margin(value, -_size.border, 0, -_size.border, 0);
        _geom.g3 = dtk::margin(_label->getGeometry(), -_size.border, 0, -_size.border, 0);
        _selectionRect = _geom.g3;
    }

    dtk::Box2I ClipItem::getChildrenClipRect() const
    {
        return _geom.g2;
    }

    void ClipItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
        }
        _setSizeHint(_layout->getSizeHint());
    }
    
    void ClipItem::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItem::drawEvent(drawRect, event);
        event.render->drawRect(
            _geom.g3,
            _selected ? event.style->getColorRole(dtk::ColorRole::Yellow) : _color);
    }

    void ClipItem::_timeUnitsUpdate()
    {
        _textUpdate();
    }

    void ClipItem::_buildMenu(const std::shared_ptr<dtk::Menu>& menu)
    {
        if (auto externalReference = dynamic_cast<OTIO_NS::ExternalReference*>(_clip->media_reference()))
        {
            auto action = std::make_shared<dtk::Action>(
                "Open Media",
                [this, externalReference]
                {
                    auto file = _file.lock();
                    const std::filesystem::path path = file->getTimelineWrapper()->getMediaPath(externalReference->target_url());
                    auto app = _app.lock();
                    app->open(path);
                });
            menu->addItem(action);
            menu->addDivider();
        }
        else if (auto sequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(_clip->media_reference()))
        {
            auto action = std::make_shared<dtk::Action>(
                "Open Image Sequence",
                [this, sequenceRef]
                {
                    auto file = _file.lock();
                    const std::string path = getSequenceFrame(
                        file->getTimelineWrapper()->getMediaPath(sequenceRef->target_url_base()),
                        sequenceRef->name_prefix(),
                        sequenceRef->start_frame(),
                        sequenceRef->frame_zero_padding(),
                        sequenceRef->name_suffix());
                    auto app = _app.lock();
                    app->open(path);
                });
            menu->addItem(action);
            menu->addDivider();
        }
        IItem::_buildMenu(menu);
    }

    void ClipItem::_textUpdate()
    {
        if (_label)
        {
            std::string text = toString(_timeRange.duration(), _timeUnits);
            _label->setDuration(text);
        }
    }
}
