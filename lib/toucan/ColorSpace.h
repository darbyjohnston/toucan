// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Color convert effect.
    class ColorConvertEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "ColorConvertEffect";
            static int constexpr version = 1;
        };

        ColorConvertEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~ColorConvertEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        std::string _fromSpace;
        std::string _toSpace;
        bool        _unpremult = true;
        std::string _contextKey;
        std::string _contextValue;
        std::string _colorConfig;
    };

    //! Premultiply alpha effect.
    class PremultEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "PremultEffect";
            static int constexpr version = 1;
        };

        PremultEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~PremultEffect();
    };

    //! Un-premultiply alpha effect.
    class UnpremultEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "UnpremultEffect";
            static int constexpr version = 1;
        };

        UnpremultEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::shared_ptr<ImageHost>&,
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~UnpremultEffect();
    };
}
