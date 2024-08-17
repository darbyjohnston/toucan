// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <filesystem>

namespace toucan
{
    //! Read operation.
    class ReadOp : public IImageOp
    {
    public:
        ReadOp(
            const std::filesystem::path&,
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~ReadOp();
        
        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) override;

    private:
        std::filesystem::path _path;
    };
}
