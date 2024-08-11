// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Saturate data.
    struct SaturateData
    {
        float value = 1.F;
    };

    //! Saturate operation.
    class SaturateOp : public IImageOp
    {
    public:
        SaturateOp(
            const SaturateData& = SaturateData(),
            const OTIO_NS::RationalTime& = OTIO_NS::RationalTime(),
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~SaturateOp();

        const SaturateData& getData() const;
        void setData(const SaturateData&);

        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        SaturateData _data;
    };

    //! Saturate OTIO effect.
    class SaturateEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "SaturateEffect";
            static int constexpr version = 1;
        };

        SaturateEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        const SaturateData& getData() const;
        void setData(const SaturateData&);

        std::shared_ptr<IImageOp> createOp(
            const OTIO_NS::RationalTime& timeOffset,
            const std::vector<std::shared_ptr<IImageOp> >& inputs) override;

    protected:
        virtual ~SaturateEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        SaturateData _data;
    };
}
