// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Filter.h"

namespace toucan
{
    BlurNode::BlurNode(
        const BlurData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Blur", inputs),
        _data(data)
    {}

    BlurNode::~BlurNode()
    {}

    const BlurData& BlurNode::getData() const
    {
        return _data;
    }

    void BlurNode::setData(const BlurData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf BlurNode::exec(
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
            propSet.setDouble("radius", 0, _data.radius);
            host->filter("Toucan:Blur", input, buf, propSet);
        }
        return buf;
    }
    
    BlurEffect::BlurEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    BlurEffect::~BlurEffect()
    {}

    std::shared_ptr<IImageNode> BlurEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<BlurNode>(_data, inputs);
    }

    bool BlurEffect::read_from(Reader& reader)
    {
        double radius = 0.0;
        bool out =
            reader.read("radius", &radius) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.radius = radius;
        }
        return out;
    }

    void BlurEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("radius", static_cast<double>(_data.radius));
    }

    ColorMapNode::ColorMapNode(
        const ColorMapData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("ColorMap", inputs),
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
        IImageNode("Invert", inputs)
    {}

    InvertNode::~InvertNode()
    {}

    OIIO::ImageBuf InvertNode::exec(
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

    PowNode::PowNode(
        const PowData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Pow", inputs),
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
        IImageNode("Saturate", inputs),
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

    UnsharpMaskNode::UnsharpMaskNode(
        const UnsharpMaskData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("UnsharpMask", inputs),
        _data(data)
    {}

    UnsharpMaskNode::~UnsharpMaskNode()
    {}

    const UnsharpMaskData& UnsharpMaskNode::getData() const
    {
        return _data;
    }

    void UnsharpMaskNode::setData(const UnsharpMaskData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf UnsharpMaskNode::exec(
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
            propSet.setString("kernal", 0, _data.kernel.c_str());
            propSet.setDouble("width", 0, _data.width);
            propSet.setDouble("contrast", 0, _data.contrast);
            propSet.setDouble("threshold", 0, _data.threshold);
            host->filter("Toucan:UnsharpMask", input, buf, propSet);
        }
        return buf;
    }

    UnsharpMaskEffect::UnsharpMaskEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    UnsharpMaskEffect::~UnsharpMaskEffect()
    {}

    std::shared_ptr<IImageNode> UnsharpMaskEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<UnsharpMaskNode>(_data, inputs);
    }

    bool UnsharpMaskEffect::read_from(Reader& reader)
    {
        std::string kernel;
        double width = 0.0;
        double contrast = 0.0;
        double threshold = 0.0;
        bool out =
            reader.read("kernel", &kernel) &&
            reader.read("width", &width) &&
            reader.read("contrast", &contrast) &&
            reader.read("threshold", &threshold) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.kernel = kernel;
            _data.width = width;
            _data.contrast = contrast;
            _data.threshold = threshold;
        }
        return out;
    }

    void UnsharpMaskEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("kernel", _data.kernel);
        writer.write("width", static_cast<double>(_data.width));
        writer.write("contrast", static_cast<double>(_data.contrast));
        writer.write("threshold", static_cast<double>(_data.threshold));
    }
}
