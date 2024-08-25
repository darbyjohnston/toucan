// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Filter.h"

#include "ImageEffectHost.h"

namespace toucan
{    
    BlurEffect::BlurEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    BlurEffect::~BlurEffect()
    {}

    std::shared_ptr<IImageNode> BlurEffect::createNode(
        const std::shared_ptr<ImageEffectHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["radius"] = static_cast<double>(_radius);
        return host->createNode(effect_name(), metadata, inputs);
    }

    bool BlurEffect::read_from(Reader& reader)
    {
        double radius = 0.0;
        bool out =
            reader.read("radius", &radius) &&
            IEffect::read_from(reader);
        if (out)
        {
            _radius = radius;
        }
        return out;
    }

    void BlurEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("radius", static_cast<double>(_radius));
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
        const std::shared_ptr<ImageEffectHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["map_name"] = _mapName;
        return host->createNode(effect_name(), metadata, inputs);
    }

    bool ColorMapEffect::read_from(Reader& reader)
    {
        bool out =
            reader.read("map_name", &_mapName) &&
            IEffect::read_from(reader);
        return out;
    }

    void ColorMapEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("map_name", _mapName);
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
        const std::shared_ptr<ImageEffectHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return host->createNode(effect_name(), metadata(), inputs);
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
        const std::shared_ptr<ImageEffectHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["value"] = static_cast<double>(_value);
        return host->createNode(effect_name(), metadata, inputs);
    }

    bool PowEffect::read_from(Reader& reader)
    {
        double value = 0.0;
        bool out =
            reader.read("value", &value) &&
            IEffect::read_from(reader);
        if (out)
        {
            _value = value;
        }
        return out;
    }

    void PowEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("value", static_cast<double>(_value));
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
        const std::shared_ptr<ImageEffectHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["value"] = static_cast<double>(_value);
        return host->createNode(effect_name(), metadata, inputs);
    }

    bool SaturateEffect::read_from(Reader& reader)
    {
        double value = 0.0;
        bool out =
            reader.read("value", &value) &&
            IEffect::read_from(reader);
        if (out)
        {
            _value = value;
        }
        return out;
    }

    void SaturateEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("value", static_cast<double>(_value));
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
        const std::shared_ptr<ImageEffectHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["kernel"] = _kernel;
        metadata["width"] = static_cast<double>(_width);
        metadata["contrast"] = static_cast<double>(_contrast);
        metadata["threshold"] = static_cast<double>(_threshold);
        return host->createNode(effect_name(), metadata, inputs);
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
            _kernel = kernel;
            _width = width;
            _contrast = contrast;
            _threshold = threshold;
        }
        return out;
    }

    void UnsharpMaskEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("kernel", _kernel);
        writer.write("width", static_cast<double>(_width));
        writer.write("contrast", static_cast<double>(_contrast));
        writer.write("threshold", static_cast<double>(_threshold));
    }
}
