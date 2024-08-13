// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Box data.
    struct BoxData
    {
        IMATH_NAMESPACE::V2i pos1  = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V2i pos2  = IMATH_NAMESPACE::V2i(0, 0);
        IMATH_NAMESPACE::V4f color = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
        bool                 fill  = true;
    };

    //! Box drawing operation.
    class BoxOp : public IImageOp
    {
    public:
        BoxOp(
            const BoxData& = BoxData(),
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~BoxOp();

        const BoxData& getData() const;
        void setData(const BoxData&);

        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        BoxData _data;
    };

    //! Box drawing OTIO effect.
    class BoxEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "BoxEffect";
            static int constexpr version = 1;
        };

        BoxEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageOp> createOp(
            const std::vector<std::shared_ptr<IImageOp> >& inputs) override;

    protected:
        virtual ~BoxEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        BoxData _data;
    };
}
