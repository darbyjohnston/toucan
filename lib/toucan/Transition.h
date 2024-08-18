// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Dissolve node.
    class DissolveNode : public IImageNode
    {
    public:
        DissolveNode(
            const OTIO_NS::TimeRange&,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~DissolveNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        OTIO_NS::TimeRange _range;
    };

    //! Horizontal wipe node.
    class HorizontalWipeNode : public IImageNode
    {
    public:
        HorizontalWipeNode(
            const OTIO_NS::TimeRange&,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~HorizontalWipeNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        OTIO_NS::TimeRange _range;
    };

    //! Vertical wipe node.
    class VerticalWipeNode : public IImageNode
    {
    public:
        VerticalWipeNode(
            const OTIO_NS::TimeRange&,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~VerticalWipeNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        OTIO_NS::TimeRange _range;
    };
}
