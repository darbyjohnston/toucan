// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ReadOp.h"

namespace toucan
{
    ReadOp::ReadOp(
        const std::filesystem::path& path,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(inputs),
        _path(path)
    {}

    ReadOp::~ReadOp()
    {}

    OIIO::ImageBuf ReadOp::exec(const OTIO_NS::RationalTime&)
    {
        return OIIO::ImageBuf(_path.string());
    }
}
