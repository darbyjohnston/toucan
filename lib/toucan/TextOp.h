// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

namespace toucan
{
    //! Text data.
    struct TextData
    {
        IMATH_NAMESPACE::V2i pos      = IMATH_NAMESPACE::V2i(0, 0);
        std::string          text;
        int                  fontSize = 16;
        std::string          fontName;
        IMATH_NAMESPACE::V4f color    = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
    };

    //! Text drawing operation.
    class TextOp : public IImageOp
    {
    public:
        TextOp(
            const TextData& = TextData(),
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~TextOp();

        const TextData& getData() const;
        void setData(const TextData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;

    private:
        TextData _data;
    };

    //! Text drawing OTIO effect.
    class TextEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "TextEffect";
            static int constexpr version = 1;
        };

        TextEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageOp> createOp(
            const std::vector<std::shared_ptr<IImageOp> >& inputs) override;

    protected:
        virtual ~TextEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        TextData _data;
    };
}
