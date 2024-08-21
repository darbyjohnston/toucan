// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompTest.h"

#include <toucan/Read.h>
#include <toucan/Comp.h>

namespace toucan
{
    void compTest(
        const std::filesystem::path& path,
        const std::shared_ptr<ImageHost>& host)
    {
        std::cout << "compTest" << std::endl;
        auto fg = std::make_shared<ReadNode>(path / "Letter_A.png");
        auto bg = std::make_shared<ReadNode>(path / "Gradient.png");
        auto comp = std::make_shared<CompNode>(
            std::vector<std::shared_ptr<IImageNode> >{ fg, bg });
        comp->setPremult(true);
        auto buf = comp->exec(OTIO_NS::RationalTime(), host);
        buf.write("compTest.png");
    }
}
