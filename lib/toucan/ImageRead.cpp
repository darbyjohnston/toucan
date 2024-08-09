// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageRead.h"

namespace toucan
{
    ImageRead::~ImageRead()
    {}

    void ImageRead::setPath(const std::filesystem::path& path)
    {
        _path = path;
    }

    void ImageRead::setTime(const opentime::RationalTime& time)
    {
        _time = time;
    }

    OIIO::ImageBuf ImageRead::exec()
    {
        OIIO::ImageBuf buf(_path.string());
        return buf;
    }
}
