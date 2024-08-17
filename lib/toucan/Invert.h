// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Invert node.
    class InvertNode : public IImageNode
    {
    public:
        InvertNode(const std::vector<std::shared_ptr<IImageNode> >& = {});

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
}
