// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageCompTest.h"

#include <toucan/ImageRead.h>
#include <toucan/ImageComp.h>

namespace toucan
{
    void imageCompTest(const std::filesystem::path& path)
    {
        std::cout << "imageCompTest" << std::endl;

        auto fg = std::make_shared<ImageRead>();
        fg->setPath(path / "Letter_A.png");
        auto bg = std::make_shared<ImageRead>();
        bg->setPath(path / "Color_Blue.png");
        auto comp = std::make_shared<ImageComp>();
        comp->setInputs({ fg, bg });
        comp->setPremult(true);
        auto buf = comp->exec();
        buf.write("ImageCompTest.png");
    }
}
