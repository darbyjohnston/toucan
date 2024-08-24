// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Draw.h"

#include "ImageHost.h"
#include "Util.h"

namespace toucan
{
    BoxEffect::BoxEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    BoxEffect::~BoxEffect()
    {}

    std::shared_ptr<IImageNode> BoxEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["pos1"] = vecToAny(_pos1);
        metadata["pos2"] = vecToAny(_pos2);
        metadata["color"] = vecToAny(_color);
        metadata["fill"] = _fill;
        return host->createNode(name(), metadata, inputs);
    }

    bool BoxEffect::read_from(Reader& reader)
    {
        OTIO_NS::AnyVector pos1;
        OTIO_NS::AnyVector pos2;
        OTIO_NS::AnyVector color;
        bool out =
            reader.read("pos1", &pos1) &&
            reader.read("pos2", &pos2) &&
            reader.read("color", &color) &&
            reader.read("fill", &_fill) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(pos1, _pos1);
            anyToVec(pos2, _pos2);
            anyToVec(color, _color);
        }
        return out;
    }

    void BoxEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("pos1", vecToAny(_pos1));
        writer.write("pos2", vecToAny(_pos2));
        writer.write("color", vecToAny(_color));
        writer.write("fill", _fill);
    }

    LineEffect::LineEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    LineEffect::~LineEffect()
    {}

    std::shared_ptr<IImageNode> LineEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["pos1"] = vecToAny(_pos1);
        metadata["pos2"] = vecToAny(_pos2);
        metadata["color"] = vecToAny(_color);
        metadata["skipFirstPoint"] = _skipFirstPoint;
        return host->createNode(name(), metadata, inputs);
    }

    bool LineEffect::read_from(Reader& reader)
    {
        OTIO_NS::AnyVector pos1;
        OTIO_NS::AnyVector pos2;
        OTIO_NS::AnyVector color;
        bool out =
            reader.read("pos1", &pos1) &&
            reader.read("pos2", &pos2) &&
            reader.read("color", &color) &&
            reader.read("skip_first_point", &_skipFirstPoint) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(pos1, _pos1);
            anyToVec(pos2, _pos2);
            anyToVec(color, _color);
        }
        return out;
    }

    void LineEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("pos1", vecToAny(_pos1));
        writer.write("pos2", vecToAny(_pos2));
        writer.write("color", vecToAny(_color));
        writer.write("skip_first_point", _skipFirstPoint);
    }

    TextEffect::TextEffect(
        std::string const& name,
        std::string const& effect_name,
        OTIO_NS::AnyDictionary const& metadata) :
        IEffect(name, effect_name, metadata)
    {}

    TextEffect::~TextEffect()
    {}

    std::shared_ptr<IImageNode> TextEffect::createNode(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        OTIO_NS::AnyDictionary metadata = this->metadata();
        metadata["pos"] = vecToAny(_pos);
        metadata["text"] = _text;
        metadata["font_size"] = static_cast<int64_t>(_fontSize);
        metadata["font_name"] = _fontName;
        metadata["color"] = vecToAny(_color);
        return host->createNode(name(), metadata, inputs);
    }

    bool TextEffect::read_from(Reader& reader)
    {
        OTIO_NS::AnyVector pos;
        int64_t fontSize = 0;
        OTIO_NS::AnyVector color;
        bool out =
            reader.read("pos", &pos) &&
            reader.read("text", &_text) &&
            reader.read("font_size", &fontSize) &&
            reader.read("font_name", &_fontName) &&
            reader.read("color", &color) &&
            IEffect::read_from(reader);
        if (out)
        {
            anyToVec(pos, _pos);
            _fontSize = fontSize;
            anyToVec(color, _color);
        }
        return out;
    }

    void TextEffect::write_to(Writer& writer) const
    {
        IEffect::write_to(writer);
        writer.write("pos", vecToAny(_pos));
        writer.write("text", _text);
        writer.write("font_size", static_cast<int64_t>(_fontSize));
        writer.write("font_name", _fontName);
        writer.write("color", vecToAny(_color));
    }
}
