// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Transform.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    FlipNode::FlipNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Flip", inputs)
    {}

    FlipNode::~FlipNode()
    {}

    OIIO::ImageBuf FlipNode::exec(
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
            host->filter("Toucan:Flip", input, buf);
        }
        return buf;
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
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<FlipNode>(inputs);
    }

    bool FlipEffect::read_from(Reader& reader)
    {
        return IEffect::read_from(reader);
    }

    void FlipEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
    }

    FlopNode::FlopNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Flop", inputs)
    {}

    FlopNode::~FlopNode()
    {}

    OIIO::ImageBuf FlopNode::exec(
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
            host->filter("Toucan:Flop", input, buf);
        }
        return buf;
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
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<FlopNode>(inputs);
    }

    bool FlopEffect::read_from(Reader& reader)
    {
        return IEffect::read_from(reader);
    }

    void FlopEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
    }

    ResizeNode::ResizeNode(
        const ResizeData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Resize", inputs),
        _data(data)
    {}

    ResizeNode::~ResizeNode()
    {}

    const ResizeData& ResizeNode::getData() const
    {
        return _data;
    }

    void ResizeNode::setData(const ResizeData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf ResizeNode::exec(
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
            auto input = _inputs[0]->exec(offsetTime, host);
            auto spec = input.spec();
            spec.width = _data.size.x;
            spec.height = _data.size.y;
            buf = OIIO::ImageBuf(spec);
            PropertySet propSet;
            propSet.setIntN("size", 2, &_data.size.x);
            propSet.setString("filterName", 0, _data.filterName.c_str());
            propSet.setDouble("filterWidth", 0, _data.filterWidth);
            host->filter("Toucan:Resize", input, buf, propSet);
        }
        return buf;
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
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<ResizeNode>(_data, inputs);
    }

    bool ResizeEffect::read_from(Reader& reader)
    {
        int64_t width = 0;
        int64_t height = 0;
        double filterWidth = 0.0;
        bool out =
            reader.read("width", &width) &&
            reader.read("height", &height) &&
            reader.read("filter_name", &_data.filterName) &&
            reader.read("filter_width", &filterWidth) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.size.x = width;
            _data.size.y = height;
            _data.filterWidth = filterWidth;
        }
        return out;
    }

    void ResizeEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("width", static_cast<int64_t>(_data.size.x));
        writer.write("height", static_cast<int64_t>(_data.size.y));
        writer.write("filter_name", _data.filterName);
        writer.write("filter_width", static_cast<double>(_data.filterWidth));
    }

    RotateNode::RotateNode(
        const RotateData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Rotate", inputs),
        _data(data)
    {}

    RotateNode::~RotateNode()
    {}

    const RotateData& RotateNode::getData() const
    {
        return _data;
    }

    void RotateNode::setData(const RotateData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf RotateNode::exec(
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
            propSet.setDouble("angle", 0, _data.angle);
            propSet.setString("filterName", 0, _data.filterName.c_str());
            propSet.setDouble("filterWidth", 0, _data.filterWidth);
            host->filter("Toucan:Rotate", input, buf, propSet);
        }   
        return buf;
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
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<RotateNode>(_data, inputs);
    }

    bool RotateEffect::read_from(Reader& reader)
    {
        double angle = 0.0;
        double filterWidth = 0.0;
        bool out =
            reader.read("angle", &angle) &&
            reader.read("filter_name", &_data.filterName) &&
            reader.read("filter_width", &filterWidth) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.angle = angle;
            _data.filterWidth = filterWidth;
        }
        return out;
    }

    void RotateEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("angle", static_cast<double>(_data.angle));
        writer.write("filter_name", _data.filterName);
        writer.write("filter_width", static_cast<double>(_data.filterWidth));
    }
}
