// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Rotate data.
    struct RotateData
    {
        float       angle       = 0.F;
        std::string filterName;
        float       filterWidth = 0.F;
    };

    //! Rotate operation.
    class RotateOp : public IImageOp
    {
    public:
        RotateOp(
            const RotateData& = RotateData(),
            const OTIO_NS::RationalTime& = OTIO_NS::RationalTime(),
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~RotateOp();

        const RotateData& getData() const;
        void setData(const RotateData&);

        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        RotateData _data;
    };

    //! Rotate OTIO effect.
    class RotateEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "RotateEffect";
            static int constexpr version = 1;
        };

        RotateEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageOp> createOp(
            const OTIO_NS::RationalTime& timeOffset,
            const std::vector<std::shared_ptr<IImageOp> >& inputs) override;

    protected:
        virtual ~RotateEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        RotateData _data;
    };
}
