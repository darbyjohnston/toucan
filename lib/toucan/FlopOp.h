// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Flop operation.
    class FlopOp : public IImageOp
    {
    public:
        FlopOp(const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~FlopOp();

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

        std::shared_ptr<IImageOp> createOp(
            const std::vector<std::shared_ptr<IImageOp> >& inputs) override;

    protected:
        virtual ~FlopEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };
}
