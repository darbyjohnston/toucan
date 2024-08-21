// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Blur data.
    struct BlurData
    {
        float radius = 10.F;
    };

    //! Blur node.
    class BlurNode : public IImageNode
    {
    public:
        BlurNode(
            const BlurData& = BlurData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~BlurNode();

        const BlurData& getData() const;
        void setData(const BlurData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;

    private:
        BlurData _data;
    };

    //! Blur OTIO effect.
    class BlurEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "BlurEffect";
            static int constexpr version = 1;
        };

        BlurEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~BlurEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        BlurData _data;
    };

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
            const ColorMapData & = ColorMapData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~ColorMapNode();

        const ColorMapData& getData() const;
        void setData(const ColorMapData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;

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
        InvertNode(const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~InvertNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;
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
    };

    //! Power data.
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
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~PowNode();

        const PowData& getData() const;
        void setData(const PowData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;

    private:
        PowData _data;
    };

    //! Power OTIO effect.
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
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~SaturateNode();

        const SaturateData& getData() const;
        void setData(const SaturateData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;

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

    //! Unsharp mask data.
    struct UnsharpMaskData
    {
        std::string kernel = "gaussian";
        float width = 3.F;
        float contrast = 1.F;
        float threshold = 0.F;
    };

    //! Unsharp mask node.
    class UnsharpMaskNode : public IImageNode
    {
    public:
        UnsharpMaskNode(
            const UnsharpMaskData & = UnsharpMaskData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~UnsharpMaskNode();

        const UnsharpMaskData& getData() const;
        void setData(const UnsharpMaskData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageHost>&) override;

    private:
        UnsharpMaskData _data;
    };

    //! Unsharp mask OTIO effect.
    class UnsharpMaskEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "UnsharpMaskEffect";
            static int constexpr version = 1;
        };

        UnsharpMaskEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~UnsharpMaskEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        UnsharpMaskData _data;
    };
}
