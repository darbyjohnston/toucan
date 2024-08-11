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
        auto fg = std::make_shared<ReadOp>();
        fg->setPath(path / "Letter_A.png");
        auto bg = std::make_shared<ReadOp>();
        bg->setPath(path / "Color_Blue.png");
        auto comp = std::make_shared<CompOp>();
        comp->setInputs({ fg, bg });
        comp->setPremult(true);
        auto buf = comp->exec();
        buf.write("compOpTest.png");
    }
}
