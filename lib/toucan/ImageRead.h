// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <filesystem>

namespace toucan
{
    //! Image reader.
    class ImageRead : public IImageOp
    {
    public:
        virtual ~ImageRead();

        void setPath(const std::filesystem::path&);

        void setTime(const OTIO_NS::RationalTime&);
        
        OIIO::ImageBuf exec() override;

    private:
        std::filesystem::path _path;
        opentime::RationalTime _time;
    };
}
