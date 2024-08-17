// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Resize data.
    struct ResizeData
    {
        IMATH_NAMESPACE::V2i size        = IMATH_NAMESPACE::V2i(0, 0);
        std::string          filterName;
        float                filterWidth = 0.F;
    };

    //! Resize node.
    class ResizeNode : public IImageNode
    {
    public:
        ResizeNode(
            const ResizeData& = ResizeData(),
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~ResizeNode();

        const ResizeData& getData() const;
        void setData(const ResizeData&);

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;

    private:
        ResizeData _data;
    };

    //! Resize OTIO effect.
    class ResizeEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "ResizeEffect";
            static int constexpr version = 1;
        };

        ResizeEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~ResizeEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;

    private:
        ResizeData _data;
    };
}
