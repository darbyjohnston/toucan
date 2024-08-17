// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Saturate data.
    struct SaturateData
    {
        float value = 1.F;
    };

    //! Saturate node.
    class SaturateNode : public IImageNode
    {
    public:
        SaturateNode(
            const SaturateData& = SaturateData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~SaturateNode();

        const SaturateData& getData() const;
        void setData(const SaturateData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

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

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~SaturateEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        SaturateData _data;
    };
}
