// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ReadImageOp.h"

namespace toucan
{
    ReadImageOp::~ReadImageOp()
    {}

    void ReadImageOp::setPath(const std::filesystem::path& path)
    {
        _path = path;
    }

    void ReadImageOp::setTime(const opentime::RationalTime& time)
    {
        _time = time;
    }

    OIIO::ImageBuf ReadImageOp::exec()
    {
        OIIO::ImageBuf buf(_path.string());
        return buf;
    }
}
