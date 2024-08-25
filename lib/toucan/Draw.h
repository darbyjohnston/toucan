// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Box drawing effect.
    class BoxEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "BoxEffect";
            static int constexpr version = 1;
        };

        BoxEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~BoxEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        IMATH_NAMESPACE::V2i _pos1 = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V2i _pos2 = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V4f _color = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
        bool _fill = true;
    };

    //! Line drawing effect.
    class LineEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "LineEffect";
            static int constexpr version = 1;
        };

        LineEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~LineEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        IMATH_NAMESPACE::V2i _pos1 = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V2i _pos2 = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V4f _color = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
        bool _skipFirstPoint = false;
    };

    //! Text drawing effect.
    class TextEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "TextEffect";
            static int constexpr version = 1;
        };

        TextEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~TextEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        IMATH_NAMESPACE::V2i _pos = IMATH_NAMESPACE::V2i(0, 0);
        std::string _text;
        int _fontSize = 16;
        std::string _fontName;
        IMATH_NAMESPACE::V4f _color = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
    };
}
