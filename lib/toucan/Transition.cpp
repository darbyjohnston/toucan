// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Transition.h"

#include <OpenImageIO/imagebufalgo.h>

#include <iostream>

namespace toucan
{
    DissolveNode::DissolveNode(
        const OTIO_NS::TimeRange& range,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Dissolve", inputs),
        _range(range)
    {}

    DissolveNode::~DissolveNode()
    {}

    OIIO::ImageBuf DissolveNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OIIO::ImageBuf buf;
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            const auto a = _inputs[0]->exec(offsetTime, host);
            const auto b = _inputs[1]->exec(offsetTime, host);
            const auto& spec = a.spec();
            buf = OIIO::ImageBuf(spec);
            PropertySet propSet;
            const double value =
                (offsetTime.rescaled_to(_range.duration().rate()) - _range.start_time()).value() /
                _range.duration().value();
            propSet.setDouble("value", 0, value);
            host->transition("Toucan:Dissolve", a, b, buf, propSet);
        }
        return buf;
    }

    HorizontalWipeNode::HorizontalWipeNode(
        const OTIO_NS::TimeRange& range,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("HorizontalWipe", inputs),
        _range(range)
    {}

    HorizontalWipeNode::~HorizontalWipeNode()
    {}

    OIIO::ImageBuf HorizontalWipeNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OIIO::ImageBuf buf;
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            const auto a = _inputs[0]->exec(offsetTime, host);
            const auto b = _inputs[1]->exec(offsetTime, host);
            const auto& spec = a.spec();
            buf = OIIO::ImageBuf(spec);
            PropertySet propSet;
            const double value =
                (offsetTime.rescaled_to(_range.duration().rate()) - _range.start_time()).value() /
                _range.duration().value();
            propSet.setDouble("value", 0, value);
            host->transition("Toucan:HorizontalWipe", a, b, buf, propSet);
        }
        return buf;
    }

    VerticalWipeNode::VerticalWipeNode(
        const OTIO_NS::TimeRange& range,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("VerticalWipe", inputs),
        _range(range)
    {}

    VerticalWipeNode::~VerticalWipeNode()
    {}

    OIIO::ImageBuf VerticalWipeNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OIIO::ImageBuf buf;
        if (_inputs.size() > 1 && _inputs[0] && _inputs[1])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            const auto a = _inputs[0]->exec(offsetTime, host);
            const auto b = _inputs[1]->exec(offsetTime, host);
            const auto& spec = a.spec();
            buf = OIIO::ImageBuf(spec);
            PropertySet propSet;
            const double value =
                (offsetTime.rescaled_to(_range.duration().rate()) - _range.start_time()).value() /
                _range.duration().value();
            propSet.setDouble("value", 0, value);
            host->transition("Toucan:VerticalWipe", a, b, buf, propSet);
        }
        return buf;
    }
}
