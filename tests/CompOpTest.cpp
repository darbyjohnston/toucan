// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompOpTest.h"

#include <toucan/ReadOp.h>
#include <toucan/CompOp.h>

namespace toucan
{
    void compOpTest(const std::filesystem::path& path)
    {
        std::cout << "compOpTest" << std::endl;
        auto fg = std::make_shared<ReadOp>(path / "Letter_A.png");
        auto bg = std::make_shared<ReadOp>(path / "Color_Blue.png");
        auto comp = std::make_shared<CompOp>(
            OTIO_NS::RationalTime(),
            std::vector<std::shared_ptr<IImageOp> >{ fg, bg });
        comp->setPremult(true);
        auto buf = comp->exec(OTIO_NS::RationalTime());
        buf.write("compOpTest.png");
    }
}
