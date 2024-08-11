// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Noise data.
    struct NoiseData
    {
        IMATH_NAMESPACE::V2i size = IMATH_NAMESPACE::V2i(0, 0);
        std::string          type = "gaussian";
        float                a    = 0.F;
        float                b    = .1F;
        bool                 mono = false;
        int                  seed = 0;
    };

    //! Noise operation.
    class NoiseOp : public IImageOp
    {
    public:
        NoiseOp(
            const NoiseData& = NoiseData(),
            const OTIO_NS::RationalTime& = OTIO_NS::RationalTime(),
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~NoiseOp();

        const NoiseData& getData() const;
        void setData(const NoiseData&);

        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        NoiseData _data;
    };

    //! Noise OTIO effect.
    class NoiseEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "NoiseEffect";
            static int constexpr version = 1;
        };

        NoiseEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageOp> createOp(
            const OTIO_NS::RationalTime& timeOffset,
            const std::vector<std::shared_ptr<IImageOp> >& inputs) override;

    protected:
        virtual ~NoiseEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        NoiseData _data;
    };
}
