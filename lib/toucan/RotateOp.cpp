// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "RotateOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    RotateOp::RotateOp(const RotateData& data) :
        _data(data)
    {}

    RotateOp::~RotateOp()
    {}

    const RotateData& RotateOp::getData() const
    {
        return _data;
    }

    void RotateOp::setData(const RotateData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf RotateOp::exec()
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            buf = OIIO::ImageBufAlgo::rotate(
                _inputs[0]->exec(),
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

    const RotateData& RotateEffect::getData() const
    {
        return _data;
    }

    void RotateEffect::setData(const RotateData & value)
    {
        _data = value;
    }

    std::shared_ptr<IImageOp> RotateEffect::createOp()
    {
        return std::make_shared<RotateOp>(_data);
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
