// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "AudioClipItem.h"

#include "App.h"
#include "File.h"

#include <toucanRender/Util.h>

#include <dtk/ui/DrawUtil.h>

#include <opentimelineio/externalReference.h>

namespace toucan
{
    void AudioClipItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const ItemData& data,
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
            data,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::SerializableObjectWithMetadata>(clip),
            timeRange,
            "toucan::AudioClipItem",
            parent);

        _clip = clip;
        _text = !clip->name().empty() ? clip->name() : "Audio Clip";
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
                    data,
                    marker,
                    markerTimeRange,
                    _markerLayout);
                _markerItems.push_back(markerItem);
            }
        }

        _textUpdate();
    }
    
    AudioClipItem::~AudioClipItem()
    {}

    std::shared_ptr<AudioClipItem> AudioClipItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const ItemData& data,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& clip,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const dtk::Color4F& color,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<AudioClipItem>();
        out->_init(context, data, clip, timeline, color, parent);
        return out;
    }

    void AudioClipItem::setScale(double value)
    {
        IItem::setScale(value);
        if (_markerLayout)
        {
            _markerLayout->setScale(value);
        }
    }

    void AudioClipItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        _layout->setGeometry(value);
        _geom.g2 = dtk::margin(value, -_size.border, 0, -_size.border, 0);
        _geom.g3 = dtk::margin(_label->getGeometry(), -_size.border, 0, -_size.border, 0);
        _selectionRect = _geom.g3;
    }

    dtk::Box2I AudioClipItem::getChildrenClipRect() const
    {
        return _geom.g2;
    }

    void AudioClipItem::sizeHintEvent(const dtk::SizeHintEvent& event)
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
    
    void AudioClipItem::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItem::drawEvent(drawRect, event);
        event.render->drawRect(
            _geom.g3,
            _selected ? event.style->getColorRole(dtk::ColorRole::Yellow) : _color);
    }

    void AudioClipItem::_timeUnitsUpdate()
    {
        _textUpdate();
    }

    void AudioClipItem::_buildMenu(const std::shared_ptr<dtk::Menu>& menu)
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
        IItem::_buildMenu(menu);
    }

    void AudioClipItem::_textUpdate()
    {
        if (_label)
        {
            std::string text = toString(_timeRange.duration(), _timeUnits);
            _label->setDuration(text);
        }
    }
}
