// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Pow data.
    struct PowData
    {
        float value = 1.F;
    };

    //! Power node.
    class PowNode : public IImageNode
    {
    public:
        PowNode(
            const PowData& = PowData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~PowNode();

        const PowData& getData() const;
        void setData(const PowData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        PowData _data;
    };

    //! Pow OTIO effect.
    class PowEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "PowEffect";
            static int constexpr version = 1;
        };

        PowEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~PowEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        PowData _data;
    };
}
