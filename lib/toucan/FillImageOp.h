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

    //! Fill image operation.
    class FillImageOp : public IImageOp
    {
    public:
        FillImageOp(const FillData& = FillData());

        virtual ~FillImageOp();

        const FillData& getData() const;
        void setData(const FillData&);

        OIIO::ImageBuf exec() override;

    private:
        FillData _data;
    };

    //! Fill image OTIO effect.
    class FillImageEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "FillImageEffect";
            static int constexpr version = 1;
        };

        FillImageEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        const FillData& getData() const;
        void setData(const FillData&);

        std::shared_ptr<IImageOp> createOp() override;

    protected:
        virtual ~FillImageEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        FillData _data;
    };
}
