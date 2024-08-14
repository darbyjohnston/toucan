// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ReadOp.h"

#include <OpenImageIO/imagebufalgo.h>

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
        OIIO::ImageBuf buf(_path.string());
        const auto& spec = buf.spec();
        if (3 == spec.nchannels)
        {
            // Add an alpha channel.
            const int channelorder[] = { 0, 1, 2, -1 };
            const float channelvalues[] = { 0, 0, 0, 1.0 };
            const std::string channelnames[] = { "", "", "", "A" };
            buf = OIIO::ImageBufAlgo::channels(buf, 4, channelorder, channelvalues, channelnames);
        }
        return buf;
    }
}
