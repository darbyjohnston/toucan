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
            const OTIO_NS::RationalTime& = OTIO_NS::RationalTime(),
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~ReadOp();
        
        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        std::filesystem::path _path;
    };
}
