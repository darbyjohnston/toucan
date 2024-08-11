// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "SaturateOp.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    SaturateOp::SaturateOp(const SaturateData& data) :
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

    OIIO::ImageBuf SaturateOp::exec()
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty())
        {
            const auto input = _inputs[0]->exec();
            const auto spec = input.spec();
            //! \todo The ROI is not working?
            buf = OIIO::ImageBufAlgo::saturate(
                input,
                _data.value);
                //0,
                //OIIO::ROI(0, spec.width, 0, spec.height, 0, 1, 0, 3));
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

    const SaturateData& SaturateEffect::getData() const
    {
        return _data;
    }

    void SaturateEffect::setData(const SaturateData & value)
    {
        _data = value;
    }

    std::shared_ptr<IImageOp> SaturateEffect::createOp()
    {
        return std::make_shared<SaturateOp>(_data);
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
