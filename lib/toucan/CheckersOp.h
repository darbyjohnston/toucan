// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Checkers data.
    struct CheckersData
    {
        IMATH_NAMESPACE::V2i size = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V2i checkerSize = IMATH_NAMESPACE::V2i(100, 100);
        IMATH_NAMESPACE::V4f color1 = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
        IMATH_NAMESPACE::V4f color2 = IMATH_NAMESPACE::V4f(0.F, 0.F, 0.F, 1.F);
    };

    //! Checkers operation.
    class CheckersOp : public IImageOp
    {
    public:
        CheckersOp(const CheckersData& = CheckersData());

        virtual ~CheckersOp();

        const CheckersData& getData() const;
        void setData(const CheckersData&);

        OIIO::ImageBuf exec() override;

    private:
        CheckersData _data;
    };

    //! Checkers OTIO effect.
    class CheckersEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "CheckersEffect";
            static int constexpr version = 1;
        };

        CheckersEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        const CheckersData& getData() const;
        void setData(const CheckersData&);

        std::shared_ptr<IImageOp> createOp() override;

    protected:
        virtual ~CheckersEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        CheckersData _data;
    };
}
