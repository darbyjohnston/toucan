// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

namespace toucan
{
    //! Linear time warp node.
    class LinearTimeWarpNode : public IImageNode
    {
    public:
        LinearTimeWarpNode(
            float timeScalar,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~LinearTimeWarpNode();
        
        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;

    private:
        float _timeScalar = 1.F;
    };
}
