// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Generator.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    CheckersNode::CheckersNode(
        const CheckersData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    CheckersNode::~CheckersNode()
    {}

    const CheckersData& CheckersNode::getData() const
    {
        return _data;
    }

    void CheckersNode::setData(const CheckersData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf CheckersNode::exec(
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
            buf = _inputs[0]->exec(offsetTime, host);
            OIIO::ImageBufAlgo::checker(
                buf,
                _data.checkerSize.x,
                _data.checkerSize.y,
                1,
                { _data.color1.x, _data.color1.y, _data.color1.z, _data.color1.w },
                { _data.color2.x, _data.color2.y, _data.color2.z, _data.color2.w });
        }
        else
        {
            buf = OIIO::ImageBufAlgo::checker(
                _data.checkerSize.x,
                _data.checkerSize.y,
                1,
                { _data.color1.x, _data.color1.y, _data.color1.z, _data.color1.w },
                { _data.color2.x, _data.color2.y, _data.color2.z, _data.color2.w },
                0,
                0,
                0,
                OIIO::ROI(0, _data.size.x, 0, _data.size.y, 0, 1, 0, 4));
        }
        return buf;
    }

    CheckersEffect::CheckersEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    CheckersEffect::~CheckersEffect()
    {}

    std::shared_ptr<IImageNode> CheckersEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<CheckersNode>(_data, inputs);
    }

    bool CheckersEffect::read_from(Reader& reader)
    {
        //! \todo What is a better way to serialize non-POD types?
        OTIO_NS::AnyVector size;
        OTIO_NS::AnyVector checkerSize;
        OTIO_NS::AnyVector color1;
        OTIO_NS::AnyVector color2;
        bool out =
            reader.read("size", &size) &&
            reader.read("checkerSize", &checkerSize) &&
            reader.read("color1", &color1) &&
            reader.read("color2", &color2) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(size, _data.size);
            anyToVec(checkerSize, _data.checkerSize);
            anyToVec(color1, _data.color1);
            anyToVec(color2, _data.color2);
        }
        return out;
    }

    void CheckersEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("size", vecToAny(_data.size));
        writer.write("checkerSize", vecToAny(_data.checkerSize));
        writer.write("color1", vecToAny(_data.color1));
        writer.write("color2", vecToAny(_data.color2));
    }

    FillNode::FillNode(
        const FillData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    FillNode::~FillNode()
    {}

    const FillData& FillNode::getData() const
    {
        return _data;
    }

    void FillNode::setData(const FillData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf FillNode::exec(
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
            buf = _inputs[0]->exec(offsetTime, host);
            OIIO::ImageBufAlgo::fill(
                buf,
                { _data.color.x, _data.color.y, _data.color.z, _data.color.w });
        }
        else
        {
            buf = OIIO::ImageBufAlgo::fill(
                { _data.color.x, _data.color.y, _data.color.z, _data.color.w },
                OIIO::ROI(0, _data.size.x, 0, _data.size.y, 0, 1, 0, 4));
        }
        return buf;
    }
    
    FillEffect::FillEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    FillEffect::~FillEffect()
    {}

    std::shared_ptr<IImageNode> FillEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<FillNode>(_data, inputs);
    }

    bool FillEffect::read_from(Reader& reader)
    {
        OTIO_NS::AnyVector size;
        OTIO_NS::AnyVector color;
        bool out =
            reader.read("size", &size) &&
            reader.read("color", &color) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(size, _data.size);
            anyToVec(color, _data.color);
        }
        return out;
    }

    void FillEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("size", vecToAny(_data.size));
        writer.write("color", vecToAny(_data.color));
    }

    NoiseNode::NoiseNode(
        const NoiseData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(inputs),
        _data(data)
    {}

    NoiseNode::~NoiseNode()
    {}

    const NoiseData& NoiseNode::getData() const
    {
        return _data;
    }

    void NoiseNode::setData(const NoiseData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf NoiseNode::exec(
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
            buf = _inputs[0]->exec(offsetTime, host);
            OIIO::ImageBufAlgo::noise(
                buf,
                _data.type,
                _data.a,
                _data.b,
                _data.mono,
                _data.seed);
        }
        else
        {
            buf = OIIO::ImageBufAlgo::noise(
                _data.type,
                _data.a,
                _data.b,
                _data.mono,
                _data.seed,
                OIIO::ROI(0, _data.size.x, 0, _data.size.y, 0, 1, 0, 4));
        }
        return buf;
    }

    NoiseEffect::NoiseEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    NoiseEffect::~NoiseEffect()
    {}

    std::shared_ptr<IImageNode> NoiseEffect::createNode(
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        return std::make_shared<NoiseNode>(_data, inputs);
    }

    bool NoiseEffect::read_from(Reader& reader)
    {
        int64_t width = 0;
        int64_t height = 0;
        double a = 0.0;
        double b = 0.0;
        bool mono = false;
        int64_t seed = 0;
        IMATH_NAMESPACE::V4d color;
        bool out =
            reader.read("width", &width) &&
            reader.read("height", &height) &&
            reader.read("type", &_data.type) &&
            reader.read("a", &a) &&
            reader.read("b", &b) &&
            reader.read("mono", &mono) &&
            reader.read("seed", &seed) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.size.x = width;
            _data.size.y = height;
            _data.a = a;
            _data.b = b;
            _data.mono = mono;
            _data.seed = seed;
        }
        return out;
    }

    void NoiseEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("width", static_cast<int64_t>(_data.size.x));
        writer.write("height", static_cast<int64_t>(_data.size.y));
        writer.write("type", _data.type);
        writer.write("a", static_cast<double>(_data.a));
        writer.write("b", static_cast<double>(_data.b));
        writer.write("mono", _data.mono);
        writer.write("seed", static_cast<int64_t>(_data.seed));
    }
}
