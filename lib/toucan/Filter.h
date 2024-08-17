// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Color map data.
    struct ColorMapData
    {
        std::string mapName;
    };

    //! Color map node.
    class ColorMapNode : public IImageNode
    {
    public:
        ColorMapNode(
            const ColorMapData& = ColorMapData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~ColorMapNode();

        const ColorMapData& getData() const;
        void setData(const ColorMapData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        ColorMapData _data;
    };

    //! Color map OTIO effect.
    class ColorMapEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "ColorMapEffect";
            static int constexpr version = 1;
        };

        ColorMapEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~ColorMapEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        ColorMapData _data;
    };

    //! Invert node.
    class InvertNode : public IImageNode
    {
    public:
        InvertNode(const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~InvertNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;
    };

    //! Invert OTIO effect.
    class InvertEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "InvertEffect";
            static int constexpr version = 1;
        };

        InvertEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~InvertEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };

    //! Pow data.
    struct PowData
    {
        float value = 1.F;
    };

    //! Power node.
    class PowNode : public IImageNode
    {
    public:
        PowNode(
            const PowData & = PowData(),
            const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~PowNode();

        const PowData& getData() const;
        void setData(const PowData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        PowData _data;
    };

    //! Pow OTIO effect.
    class PowEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "PowEffect";
            static int constexpr version = 1;
        };

        PowEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~PowEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        PowData _data;
    };

    //! Saturate data.
    struct SaturateData
    {
        float value = 1.F;
    };

    //! Saturate node.
    class SaturateNode : public IImageNode
    {
    public:
        SaturateNode(
            const SaturateData & = SaturateData(),
            const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~SaturateNode();

        const SaturateData& getData() const;
        void setData(const SaturateData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        SaturateData _data;
    };

    //! Saturate OTIO effect.
    class SaturateEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "SaturateEffect";
            static int constexpr version = 1;
        };

        SaturateEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~SaturateEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        SaturateData _data;
    };
}
