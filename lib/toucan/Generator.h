// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Checkers data.
    struct CheckersData
    {
        IMATH_NAMESPACE::V2i size = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V2i checkerSize = IMATH_NAMESPACE::V2i(100, 100);
        IMATH_NAMESPACE::V4f color1 = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
        IMATH_NAMESPACE::V4f color2 = IMATH_NAMESPACE::V4f(0.F, 0.F, 0.F, 1.F);
    };

    //! Checkers node.
    class CheckersNode : public IImageNode
    {
    public:
        CheckersNode(
            const CheckersData & = CheckersData(),
            const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~CheckersNode();

        const CheckersData& getData() const;
        void setData(const CheckersData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        CheckersData _data;
    };

    //! Checkers OTIO effect.
    class CheckersEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "CheckersEffect";
            static int constexpr version = 1;
        };

        CheckersEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~CheckersEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        CheckersData _data;
    };

    //! Fill data.
    struct FillData
    {
        IMATH_NAMESPACE::V2i size  = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V4f color = IMATH_NAMESPACE::V4f(0.F, 0.F, 0.F, 0.F);
    };

    //! Fill node.
    class FillNode : public IImageNode
    {
    public:
        FillNode(
            const FillData& = FillData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~FillNode();

        const FillData& getData() const;
        void setData(const FillData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        FillData _data;
    };

    //! Fill OTIO effect.
    class FillEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "FillEffect";
            static int constexpr version = 1;
        };

        FillEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~FillEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        FillData _data;
    };

    //! Noise data.
    struct NoiseData
    {
        IMATH_NAMESPACE::V2i size = IMATH_NAMESPACE::V2i(0, 0);
        std::string          type = "gaussian";
        float                a = 0.F;
        float                b = .1F;
        bool                 mono = false;
        int                  seed = 0;
    };

    //! Noise node.
    class NoiseNode : public IImageNode
    {
    public:
        NoiseNode(
            const NoiseData & = NoiseData(),
            const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~NoiseNode();

        const NoiseData& getData() const;
        void setData(const NoiseData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        NoiseData _data;
    };

    //! Noise OTIO effect.
    class NoiseEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "NoiseEffect";
            static int constexpr version = 1;
        };

        NoiseEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~NoiseEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        NoiseData _data;
    };
}
