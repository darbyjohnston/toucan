// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ColorSpace.h"

#include "ImageHost.h"

namespace toucan
{
    ColorConvertEffect::ColorConvertEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    ColorConvertEffect::~ColorConvertEffect()
    {}

    std::shared_ptr<IImageNode> ColorConvertEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["fromspace"] = _fromSpace;
        metadata["tospace"] = _toSpace;
        metadata["unpremult"] = _unpremult;
        metadata["context_key"] = _contextKey;
        metadata["context_value"] = _contextValue;
        metadata["color_config"] = _colorConfig;
        return host->createNode(name(), metadata, inputs);
    }

    bool ColorConvertEffect::read_from(Reader& reader)
    {
        bool out =
            reader.read("fromspace", &_fromSpace) &&
            reader.read("tospace", &_toSpace) &&
            reader.read("unpremult", &_unpremult) &&
            reader.read("context_key", &_contextKey) &&
            reader.read("context_value", &_contextValue) &&
            reader.read("color_config", &_colorConfig) &&
            IEffect::read_from(reader);
        return out;
    }

    void ColorConvertEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("fromspace", _fromSpace);
        writer.write("tospace", _toSpace);
        writer.write("unpremult", _unpremult);
        writer.write("context_key", _contextKey);
        writer.write("context_value", _contextValue);
        writer.write("color_config", _colorConfig);
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
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return host->createNode(name(), metadata(), inputs);
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
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return host->createNode(name(), metadata(), inputs);
    }
}
