// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/ImageNode.h>

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
