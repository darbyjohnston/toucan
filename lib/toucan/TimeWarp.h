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
        
        OIIO::ImageBuf exec() override;

    private:
        float _timeScalar = 1.F;
    };
}
