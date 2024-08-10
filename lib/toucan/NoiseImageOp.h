// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <filesystem>

namespace toucan
{
    //! Noise data.
    struct NoiseData
    {
        IMATH_NAMESPACE::V2i size = IMATH_NAMESPACE::V2i(0, 0);
        std::string type = "gaussian";
        float a = 0.F;
        float b = .1F;
        bool mono = false;
        int seed = 0;
    };

    //! Noise image operation.
    class NoiseImageOp : public IImageOp
    {
    public:
        NoiseImageOp(const NoiseData& = NoiseData());

        virtual ~NoiseImageOp();

        const NoiseData& getData() const;
        void setData(const NoiseData&);

        OIIO::ImageBuf exec() override;

    private:
        NoiseData _data;
    };

    //! Noise image OTIO effect.
    class NoiseImageEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "NoiseImageEffect";
            static int constexpr version = 1;
        };

        NoiseImageEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        const NoiseData& getData() const;
        void setData(const NoiseData&);

        std::shared_ptr<IImageOp> createOp() override;

    protected:
        virtual ~NoiseImageEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        NoiseData _data;
    };
}
