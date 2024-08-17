// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Flip operation.
    class FlipOp : public IImageOp
    {
    public:
        FlipOp(const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~FlipOp();

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

        std::shared_ptr<IImageOp> createOp(
            const std::vector<std::shared_ptr<IImageOp> >& inputs) override;

    protected:
        virtual ~FlipEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };
}
