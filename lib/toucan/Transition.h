// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Transition node.
    class TransitionNode : public IImageNode
    {
    public:
        TransitionNode(
            const OTIO_NS::TimeRange&,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~TransitionNode();
        
        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        OTIO_NS::TimeRange _range;
    };
}
