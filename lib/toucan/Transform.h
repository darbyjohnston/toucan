// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Flip node.
    class FlipNode : public IImageNode
    {
    public:
        FlipNode(const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~FlipNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;
    };

    //! Flip OTIO effect.
    class FlipEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "FlipEffect";
            static int constexpr version = 1;
        };

        FlipEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~FlipEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };

    //! Flop node.
    class FlopNode : public IImageNode
    {
    public:
        FlopNode(const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~FlopNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;
    };

    //! Flop OTIO effect.
    class FlopEffect : public IEffect
    {
    public:
        struct Schema
        {
            static auto constexpr name = "FlopEffect";
            static int constexpr version = 1;
        };

        FlopEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) override;

    protected:
        virtual ~FlopEffect();

        bool read_from(Reader&) override;
        void write_to(Writer&) const override;
    };

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
            const std::shared_ptr<ImageEffectHost>&) override;

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

    //! Rotate data.
    struct RotateData
    {
        float       angle = 0.F;
        std::string filterName;
        float       filterWidth = 0.F;
    };

    //! Rotate node.
    class RotateNode : public IImageNode
    {
    public:
        RotateNode(
            const RotateData & = RotateData(),
            const std::vector<std::shared_ptr<IImageNode> > & = {});

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
