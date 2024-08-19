// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Premultiply alpha node.
    class PremultNode : public IImageNode
    {
    public:
        PremultNode(const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~PremultNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;
    };

    //! Premultiply alpha OTIO effect.
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
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~PremultEffect();
    };

    //! Un-premultiply alpha node.
    class UnpremultNode : public IImageNode
    {
    public:
        UnpremultNode(const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~UnpremultNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;
    };

    //! Un-premultiply alpha OTIO effect.
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
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~UnpremultEffect();
    };
}
