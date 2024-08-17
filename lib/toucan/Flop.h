// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Flop node.
    class FlopNode : public IImageNode
    {
    public:
        FlopNode(const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~FlopNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;
    };

    //! Flop OTIO effect.
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
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~FlopEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };
}
