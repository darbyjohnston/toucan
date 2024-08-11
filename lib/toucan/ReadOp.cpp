// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ReadOp.h"

namespace toucan
{
    ReadOp::~ReadOp()
    {}

    void ReadOp::setPath(const std::filesystem::path& path)
    {
        _path = path;
    }

    void ReadOp::setTime(const opentime::RationalTime& time)
    {
        _time = time;
    }

    OIIO::ImageBuf ReadOp::exec()
    {
        OIIO::ImageBuf buf(_path.string());
        return buf;
    }
}
