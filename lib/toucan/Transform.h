// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Crop effect.
    class CropEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "CropEffect";
            static int constexpr version = 1;
        };

        CropEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~CropEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        IMATH_NAMESPACE::V2i _pos = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V2i _size = IMATH_NAMESPACE::V2i(0, 0);
    };

    //! Flip effect.
    class FlipEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "FlipEffect";
            static int constexpr version = 1;
        };

        FlipEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~FlipEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };

    //! Flop effect.
    class FlopEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "FlopEffect";
            static int constexpr version = 1;
        };

        FlopEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~FlopEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };

    //! Resize effect.
    class ResizeEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "ResizeEffect";
            static int constexpr version = 1;
        };

        ResizeEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~ResizeEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        IMATH_NAMESPACE::V2i _size = IMATH_NAMESPACE::V2i(0, 0);
        std::string _filterName;
        float _filterWidth = 0.F;
    };

    //! Rotate effect.
    class RotateEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "RotateEffect";
            static int constexpr version = 1;
        };

        RotateEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~RotateEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        float _angle = 0.F;
        std::string _filterName;
        float _filterWidth = 0.F;
    };
}
