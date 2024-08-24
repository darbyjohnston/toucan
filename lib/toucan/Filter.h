// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Blur effect.
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
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~BlurEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        float _radius = 10.F;
    };

    //! Color map effect.
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
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~ColorMapEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        std::string _mapName;
    };

    //! Invert effect.
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
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~InvertEffect();
    };

    //! Power effect.
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
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~PowEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        float _value = 1.F;
    };

    //! Saturate effect.
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
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~SaturateEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        float _value = 1.F;
    };

    //! Unsharp mask effect.
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
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~UnsharpMaskEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        std::string _kernel = "gaussian";
        float _width = 3.F;
        float _contrast = 1.F;
        float _threshold = 0.F;
    };
}
