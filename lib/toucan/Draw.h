// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Box data.
    struct BoxData
    {
        IMATH_NAMESPACE::V2i pos1  = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V2i pos2  = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V4f color = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
        bool                 fill  = true;
    };

    //! Box drawing node.
    class BoxNode : public IImageNode
    {
    public:
        BoxNode(
            const BoxData& = BoxData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~BoxNode();

        const BoxData& getData() const;
        void setData(const BoxData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;

    private:
        BoxData _data;
    };

    //! Box drawing OTIO effect.
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
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~BoxEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        BoxData _data;
    };

    //! Line data.
    struct LineData
    {
        IMATH_NAMESPACE::V2i pos1 = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V2i pos2 = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V4f color = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
        bool skipFirstPoint = false;
    };

    //! Line drawing node.
    class LineNode : public IImageNode
    {
    public:
        LineNode(
            const LineData & = LineData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~LineNode();

        const LineData& getData() const;
        void setData(const LineData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;

    private:
        LineData _data;
    };

    //! Line drawing OTIO effect.
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
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~LineEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        LineData _data;
    };

    //! Text data.
    struct TextData
    {
        IMATH_NAMESPACE::V2i pos = IMATH_NAMESPACE::V2i(0, 0);
        std::string          text;
        int                  fontSize = 16;
        std::string          fontName;
        IMATH_NAMESPACE::V4f color = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
    };

    //! Text drawing node.
    class TextNode : public IImageNode
    {
    public:
        TextNode(
            const TextData & = TextData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~TextNode();

        const TextData& getData() const;
        void setData(const TextData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;

    private:
        TextData _data;
    };

    //! Text drawing OTIO effect.
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
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~TextEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        TextData _data;
    };
}
