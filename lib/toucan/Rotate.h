// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Rotate data.
    struct RotateData
    {
        float       angle       = 0.F;
        std::string filterName;
        float       filterWidth = 0.F;
    };

    //! Rotate node.
    class RotateNode : public IImageNode
    {
    public:
        RotateNode(
            const RotateData& = RotateData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~RotateNode();

        const RotateData& getData() const;
        void setData(const RotateData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

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

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~RotateEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        RotateData _data;
    };
}
