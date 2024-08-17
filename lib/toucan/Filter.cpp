// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Filter.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    ColorMapNode::ColorMapNode(
        const ColorMapData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    ColorMapNode::~ColorMapNode()
    {}

    const ColorMapData& ColorMapNode::getData() const
    {
        return _data;
    }

    void ColorMapNode::setData(const ColorMapData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf ColorMapNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
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
            propSet.setString("mapName", 0, _data.mapName.c_str());
            host->filter("Toucan:ColorMap", input, buf, propSet);
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

    std::shared_ptr<IImageNode> ColorMapEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<ColorMapNode>(_data, inputs);
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

    InvertNode::InvertNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs)
    {}

    InvertNode::~InvertNode()
    {}

    OIIO::ImageBuf InvertNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
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
            host->filter("Toucan:Invert", input, buf);
        }
        return buf;
    }

    InvertEffect::InvertEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    InvertEffect::~InvertEffect()
    {}

    std::shared_ptr<IImageNode> InvertEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<InvertNode>(inputs);
    }

    bool InvertEffect::read_from(Reader& reader)
    {
        return IEffect::read_from(reader);
    }

    void InvertEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
    }

    PowNode::PowNode(
        const PowData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    PowNode::~PowNode()
    {}

    const PowData& PowNode::getData() const
    {
        return _data;
    }

    void PowNode::setData(const PowData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf PowNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
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
            propSet.setDouble("value", 0, _data.value);
            host->filter("Toucan:Pow", input, buf, propSet);
        }
        return buf;
    }

    PowEffect::PowEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    PowEffect::~PowEffect()
    {}

    std::shared_ptr<IImageNode> PowEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<PowNode>(_data, inputs);
    }

    bool PowEffect::read_from(Reader& reader)
    {
        double value = 0.0;
        bool out =
            reader.read("value", &value) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.value = value;
        }
        return out;
    }

    void PowEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("value", static_cast<double>(_data.value));
    }

    SaturateNode::SaturateNode(
        const SaturateData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    SaturateNode::~SaturateNode()
    {}

    const SaturateData& SaturateNode::getData() const
    {
        return _data;
    }

    void SaturateNode::setData(const SaturateData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf SaturateNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
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
            propSet.setDouble("value", 0, _data.value);
            host->filter("Toucan:Saturate", input, buf, propSet);
        }
        return buf;
    }

    SaturateEffect::SaturateEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    SaturateEffect::~SaturateEffect()
    {}

    std::shared_ptr<IImageNode> SaturateEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<SaturateNode>(_data, inputs);
    }

    bool SaturateEffect::read_from(Reader& reader)
    {
        double value = 0.0;
        bool out =
            reader.read("value", &value) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.value = value;
        }
        return out;
    }

    void SaturateEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("value", static_cast<double>(_data.value));
    }
}
