// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageReadTest.h"

#include <toucan/ImageRead.h>

namespace toucan
{
    void imageReadTest(const std::filesystem::path& path)
    {
        auto read = std::make_shared<ImageRead>();
        read->setPath(path / "Letter_A.png");
        auto buf = read->exec();
        const auto& spec = buf.spec();
        assert(spec.width > 0);
    }
}
