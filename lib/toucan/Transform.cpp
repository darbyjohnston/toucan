// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Transform.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    FlipNode::FlipNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs)
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
            buf = OIIO::ImageBufAlgo::flip(input);
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
        IImageNode(inputs)
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
            buf = OIIO::ImageBufAlgo::flop(input);
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
        IImageNode(inputs),
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
            buf = OIIO::ImageBufAlgo::resize(
                _inputs[0]->exec(offsetTime, host),
                _data.filterName,
                _data.filterWidth,
                OIIO::ROI(0, _data.size.x, 0, _data.size.y, 0, 1, 0, 4));
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
        IImageNode(inputs),
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
            buf = OIIO::ImageBufAlgo::rotate(
                _inputs[0]->exec(offsetTime, host),
                _data.angle / 360.F * 2.F * M_PI,
                _data.filterName,
                _data.filterWidth);
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
