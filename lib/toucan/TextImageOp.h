// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <filesystem>

namespace toucan
{
    //! Text data.
    struct TextData
    {
        IMATH_NAMESPACE::V2i pos = IMATH_NAMESPACE::V2i(0, 0);
        std::string text;
        int fontSize = 16;
        std::string fontName;
        IMATH_NAMESPACE::V4f color = IMATH_NAMESPACE::V4f(1.F, 1.F, 1.F, 1.F);
    };

    //! Text drawing image operation.
    class TextImageOp : public IImageOp
    {
    public:
        TextImageOp(const TextData& = TextData());

        virtual ~TextImageOp();

        const TextData& getData() const;
        void setData(const TextData&);

        OIIO::ImageBuf exec() override;

    private:
        TextData _data;
    };

    //! Text drawing image OTIO effect.
    class TextImageEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "TextImageEffect";
            static int constexpr version = 1;
        };

        TextImageEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        const TextData& getData() const;
        void setData(const TextData&);

        std::shared_ptr<IImageOp> createOp() override;

    protected:
        virtual ~TextImageEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        TextData _data;
    };
}
