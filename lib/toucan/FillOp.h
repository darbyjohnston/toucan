// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <filesystem>

namespace toucan
{
    //! Fill data.
    struct FillData
    {
        IMATH_NAMESPACE::V2i size = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V4f color = IMATH_NAMESPACE::V4f(0.F, 0.F, 0.F, 0.F);
    };

    //! Fill operation.
    class FillOp : public IImageOp
    {
    public:
        FillOp(const FillData& = FillData());

        virtual ~FillOp();

        const FillData& getData() const;
        void setData(const FillData&);

        OIIO::ImageBuf exec() override;

    private:
        FillData _data;
    };

    //! Fill OTIO effect.
    class FillEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "FillEffect";
            static int constexpr version = 1;
        };

        FillEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        const FillData& getData() const;
        void setData(const FillData&);

        std::shared_ptr<IImageOp> createOp() override;

    protected:
        virtual ~FillEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        FillData _data;
    };
}
