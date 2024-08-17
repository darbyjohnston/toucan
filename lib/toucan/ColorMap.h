// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Color map data.
    struct ColorMapData
    {
        std::string mapName;
    };

    //! Color map node.
    class ColorMapNode : public IImageNode
    {
    public:
        ColorMapNode(
            const ColorMapData& = ColorMapData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~ColorMapNode();

        const ColorMapData& getData() const;
        void setData(const ColorMapData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        ColorMapData _data;
    };

    //! Color map OTIO effect.
    class ColorMapEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "ColorMapEffect";
            static int constexpr version = 1;
        };

        ColorMapEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~ColorMapEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        ColorMapData _data;
    };
}
