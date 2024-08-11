// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "FillOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    FillOp::FillOp(const FillData& data) :
        _data(data)
    {}

    FillOp::~FillOp()
    {}

    const FillData& FillOp::getData() const
    {
        return _data;
    }

    void FillOp::setData(const FillData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf FillOp::exec()
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            buf = _inputs[0]->exec();
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

    const FillData& FillEffect::getData() const
    {
        return _data;
    }

    void FillEffect::setData(const FillData & value)
    {
        _data = value;
    }

    std::shared_ptr<IImageOp> FillEffect::createOp()
    {
        return std::make_shared<FillOp>(_data);
    }

    bool FillEffect::read_from(Reader& reader)
    {
        int64_t width = 0;
        int64_t height = 0;
        IMATH_NAMESPACE::V4d color;
        bool out =
            reader.read("width", &width) &&
            reader.read("height", &height) &&
            reader.read("red", &color.x) &&
            reader.read("green", &color.y) &&
            reader.read("blue", &color.z) &&
            reader.read("alpha", &color.w) &&
            IEffect::read_from(reader);
        if (out)
        {
            _data.size.x = width;
            _data.size.y = height;
            _data.color = color;
        }
        return out;
    }

    void FillEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("width", static_cast<int64_t>(_data.size.x));
        writer.write("height", static_cast<int64_t>(_data.size.y));
        writer.write("red", static_cast<double>(_data.color.x));
        writer.write("green", static_cast<double>(_data.color.y));
        writer.write("blue", static_cast<double>(_data.color.z));
        writer.write("alpha", static_cast<double>(_data.color.w));
    }
}
