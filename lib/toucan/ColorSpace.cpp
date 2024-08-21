// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ColorSpace.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    ColorConvertNode::ColorConvertNode(
        const ColorConvertData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("ColorConvert", inputs),
        _data(data)
    {}

    ColorConvertNode::~ColorConvertNode()
    {}

    const ColorConvertData& ColorConvertNode::getData() const
    {
        return _data;
    }

    void ColorConvertNode::setData(const ColorConvertData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf ColorConvertNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageHost>& host)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty() && _inputs[0])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            const auto input = _inputs[0]->exec(offsetTime, host);
            const auto& spec = input.spec();
            buf = OIIO::ImageBuf(spec);
            PropertySet propSet;
            propSet.setString("fromspace", 0, _data.fromspace.c_str());
            propSet.setString("tospace", 0, _data.tospace.c_str());
            propSet.setInt("unpremult", 0, _data.unpremult);
            propSet.setString("context_key", 0, _data.context_key.c_str());
            propSet.setString("context_value", 0, _data.context_value.c_str());
            propSet.setString("color_config", 0, _data.color_config.c_str());
            host->filter("Toucan:ColorConvert", input, buf, propSet);
        }
        return buf;
    }

    ColorConvertEffect::ColorConvertEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    ColorConvertEffect::~ColorConvertEffect()
    {}

    std::shared_ptr<IImageNode> ColorConvertEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<ColorConvertNode>(_data, inputs);
    }

    bool ColorConvertEffect::read_from(Reader& reader)
    {
        bool out =
            reader.read("fromspace", &_data.fromspace) &&
            reader.read("tospace", &_data.tospace) &&
            reader.read("unpremult", &_data.unpremult) &&
            reader.read("context_key", &_data.context_key) &&
            reader.read("context_value", &_data.context_value) &&
            reader.read("color_config", &_data.color_config) &&
            IEffect::read_from(reader);
        return out;
    }

    void ColorConvertEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("fromspace", _data.fromspace);
        writer.write("tospace", _data.tospace);
        writer.write("unpremult", _data.unpremult);
        writer.write("context_key", _data.context_key);
        writer.write("context_value", _data.context_value);
        writer.write("color_config", _data.color_config);
    }

    PremultNode::PremultNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Premult", inputs)
    {}

    PremultNode::~PremultNode()
    {}

    OIIO::ImageBuf PremultNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageHost>& host)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty() && _inputs[0])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            const auto input = _inputs[0]->exec(offsetTime, host);
            const auto& spec = input.spec();
            buf = OIIO::ImageBuf(spec);
            host->filter("Toucan:Premult", input, buf);
        }
        return buf;
    }

    PremultEffect::PremultEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    PremultEffect::~PremultEffect()
    {}

    std::shared_ptr<IImageNode> PremultEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<PremultNode>(inputs);
    }

    UnpremultNode::UnpremultNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Unpremult", inputs)
    {}

    UnpremultNode::~UnpremultNode()
    {}

    OIIO::ImageBuf UnpremultNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageHost>& host)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty() && _inputs[0])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            const auto input = _inputs[0]->exec(offsetTime, host);
            const auto& spec = input.spec();
            buf = OIIO::ImageBuf(spec);
            host->filter("Toucan:Unpremult", input, buf);
        }
        return buf;
    }

    UnpremultEffect::UnpremultEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    UnpremultEffect::~UnpremultEffect()
    {}

    std::shared_ptr<IImageNode> UnpremultEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<UnpremultNode>(inputs);
    }
}
