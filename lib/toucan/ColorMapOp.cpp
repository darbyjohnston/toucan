// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ColorMapOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    ColorMapOp::ColorMapOp(
        const ColorMapData& data,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(inputs),
        _data(data)
    {}

    ColorMapOp::~ColorMapOp()
    {}

    const ColorMapData& ColorMapOp::getData() const
    {
        return _data;
    }

    void ColorMapOp::setData(const ColorMapData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf ColorMapOp::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<Host>& host)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty() && _inputs[0])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            buf = OIIO::ImageBufAlgo::color_map(
                _inputs[0]->exec(offsetTime, host),
                -1,
                _data.mapName);

            // Add an alpha channel.
            const int channelorder[] = { 0, 1, 2, -1 };
            const float channelvalues[] = { 0, 0, 0, 1.0 };
            const std::string channelnames[] = { "", "", "", "A" };
            buf = OIIO::ImageBufAlgo::channels(buf, 4, channelorder, channelvalues, channelnames);
        }
        return buf;
    }
    
    ColorMapEffect::ColorMapEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    ColorMapEffect::~ColorMapEffect()
    {}

    std::shared_ptr<IImageOp> ColorMapEffect::createOp(
        const std::vector<std::shared_ptr<IImageOp> >& inputs)
    {
        return std::make_shared<ColorMapOp>(_data, inputs);
    }

    bool ColorMapEffect::read_from(Reader& reader)
    {
        bool out =
            reader.read("map_name", &_data.mapName) &&
            IEffect::read_from(reader);
        return out;
    }

    void ColorMapEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("map_name", _data.mapName);
    }
}
