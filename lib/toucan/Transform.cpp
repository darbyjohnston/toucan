// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Transform.h"

#include "ImageHost.h"
#include "Util.h"

namespace toucan
{
    CropEffect::CropEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    CropEffect::~CropEffect()
    {}

    std::shared_ptr<IImageNode> CropEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["pos"] = vecToAny(_pos);
        metadata["size"] = vecToAny(_size);
        return host->createNode(name(), metadata, inputs);
    }

    bool CropEffect::read_from(Reader& reader)
    {
        OTIO_NS::AnyVector pos;
        OTIO_NS::AnyVector size;
        bool out =
            reader.read("pos", &pos) &&
            reader.read("size", &size) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(pos, _pos);
            anyToVec(size, _size);
        }
        return out;
    }

    void CropEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("pos", vecToAny(_pos));
        writer.write("size", vecToAny(_size));
    }

    FlipEffect::FlipEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    FlipEffect::~FlipEffect()
    {}

    std::shared_ptr<IImageNode> FlipEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return host->createNode(name(), metadata(), inputs);
    }

    bool FlipEffect::read_from(Reader& reader)
    {
        return IEffect::read_from(reader);
    }

    void FlipEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
    }

    FlopEffect::FlopEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    FlopEffect::~FlopEffect()
    {}

    std::shared_ptr<IImageNode> FlopEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return host->createNode(name(), metadata(), inputs);
    }

    bool FlopEffect::read_from(Reader& reader)
    {
        return IEffect::read_from(reader);
    }

    void FlopEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
    }
    
    ResizeEffect::ResizeEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    ResizeEffect::~ResizeEffect()
    {}

    std::shared_ptr<IImageNode> ResizeEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["size"] = vecToAny(_size);
        metadata["filter_name"] = _filterName;
        metadata["filter_width"] = static_cast<double>(_filterWidth);
        return host->createNode(name(), metadata, inputs);
    }

    bool ResizeEffect::read_from(Reader& reader)
    {
        OTIO_NS::AnyVector size;
        double filterWidth = 0.0;
        bool out =
            reader.read("size", &size) &&
            reader.read("filter_name", &_filterName) &&
            reader.read("filter_width", &filterWidth) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(size, _size);
            _filterWidth = filterWidth;
        }
        return out;
    }

    void ResizeEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("size", vecToAny(_size));
        writer.write("filter_name", _filterName);
        writer.write("filter_width", static_cast<double>(_filterWidth));
    }

    RotateEffect::RotateEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    RotateEffect::~RotateEffect()
    {}

    std::shared_ptr<IImageNode> RotateEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["angle"] = static_cast<double>(_angle);
        metadata["filter_name"] = _filterName;
        metadata["filter_width"] = static_cast<double>(_filterWidth);
        return host->createNode(name(), metadata, inputs);
    }

    bool RotateEffect::read_from(Reader& reader)
    {
        double angle = 0.0;
        double filterWidth = 0.0;
        bool out =
            reader.read("angle", &angle) &&
            reader.read("filter_name", &_filterName) &&
            reader.read("filter_width", &filterWidth) &&
            IEffect::read_from(reader);
        if (out)
        {
            _angle = angle;
            _filterWidth = filterWidth;
        }
        return out;
    }

    void RotateEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("angle", static_cast<double>(_angle));
        writer.write("filter_name", _filterName);
        writer.write("filter_width", static_cast<double>(_filterWidth));
    }
}
