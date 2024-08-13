// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TextOp.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    TextOp::TextOp(
        const TextData& data,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(inputs),
        _data(data)
    {}

    TextOp::~TextOp()
    {}

    const TextData& TextOp::getData() const
    {
        return _data;
    }

    void TextOp::setData(const TextData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf TextOp::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf buf;
        if (!_inputs.empty() && _inputs[0])
        {
            OTIO_NS::RationalTime offsetTime = time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }
            buf = _inputs[0]->exec(offsetTime);
            OIIO::ImageBufAlgo::render_text(
                buf,
                _data.pos.x,
                _data.pos.y,
                _data.text,
                _data.fontSize,
                _data.fontName,
                { _data.color.x, _data.color.y, _data.color.z, _data.color.w });
        }
        return buf;
    }
    
    TextEffect::TextEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    TextEffect::~TextEffect()
    {}

    std::shared_ptr<IImageOp> TextEffect::createOp(
        const std::vector<std::shared_ptr<IImageOp> >& inputs)
    {
        return std::make_shared<TextOp>(_data, inputs);
    }

    bool TextEffect::read_from(Reader& reader)
    {
        OTIO_NS::AnyVector pos;
        int64_t fontSize = 0;
        OTIO_NS::AnyVector color;
        bool out =
            reader.read("pos", &pos) &&
            reader.read("text", &_data.text) &&
            reader.read("font_size", &fontSize) &&
            reader.read("font_name", &_data.fontName) &&
            reader.read("color", &color) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(pos, _data.pos);
            _data.fontSize = fontSize;
            anyToVec(color, _data.color);
        }
        return out;
    }

    void TextEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("pos", vecToAny(_data.pos));
        writer.write("text", _data.text);
        writer.write("font_size", static_cast<int64_t>(_data.fontSize));
        writer.write("font_name", _data.fontName);
        writer.write("color", vecToAny(_data.color));
    }
}
