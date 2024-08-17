// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

#include <filesystem>

namespace toucan
{
    //! Read node.
    class ReadNode : public IImageNode
    {
    public:
        ReadNode(
            const std::filesystem::path&,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~ReadNode();
        
        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;

    private:
        std::filesystem::path _path;
    };
}
