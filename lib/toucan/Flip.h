// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Flip node.
    class FlipNode : public IImageNode
    {
    public:
        FlipNode(const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~FlipNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;
    };

    //! Flip OTIO effect.
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
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~FlipEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };
}
