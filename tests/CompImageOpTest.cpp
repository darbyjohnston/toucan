// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompImageOpTest.h"

#include <toucan/ReadImageOp.h>
#include <toucan/CompImageOp.h>

namespace toucan
{
    void compImageOpTest(const std::filesystem::path& path)
    {
        std::cout << "compImageOpTest" << std::endl;
        auto fg = std::make_shared<ReadImageOp>();
        fg->setPath(path / "Letter_A.png");
        auto bg = std::make_shared<ReadImageOp>();
        bg->setPath(path / "Color_Blue.png");
        auto comp = std::make_shared<CompImageOp>();
        comp->setInputs({ fg, bg });
        comp->setPremult(true);
        auto buf = comp->exec();
        buf.write("ImageCompTest.png");
    }
}
