// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "SaturateOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    SaturateOp::SaturateOp(
        const SaturateData& data,
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(timeOffset, inputs),
        _data(data)
    {}

    SaturateOp::~SaturateOp()
    {}

    const SaturateData& SaturateOp::getData() const
    {
        return _data;
    }

    void SaturateOp::setData(const SaturateData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf SaturateOp::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            buf = OIIO::ImageBufAlgo::saturate(
                _inputs[0]->exec(time),
                _data.value);
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

    std::shared_ptr<IImageOp> SaturateEffect::createOp(
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs)
    {
        return std::make_shared<SaturateOp>(_data, timeOffset, inputs);
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
}
