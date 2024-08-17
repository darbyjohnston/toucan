// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "NoiseOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    NoiseOp::NoiseOp(
        const NoiseData& data,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(inputs),
        _data(data)
    {}

    NoiseOp::~NoiseOp()
    {}

    const NoiseData& NoiseOp::getData() const
    {
        return _data;
    }

    void NoiseOp::setData(const NoiseData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf NoiseOp::exec(
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

    std::shared_ptr<IImageOp> NoiseEffect::createOp(
        const std::vector<std::shared_ptr<IImageOp> >& inputs)
    {
        return std::make_shared<NoiseOp>(_data, inputs);
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
