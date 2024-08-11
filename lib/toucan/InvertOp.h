// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Invert operation.
    class InvertOp : public IImageOp
    {
    public:
        InvertOp(const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~InvertOp();

        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;
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

        std::shared_ptr<IImageOp> createOp(
            const std::vector<std::shared_ptr<IImageOp> >& inputs) override;

    protected:
        virtual ~InvertEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };
}
