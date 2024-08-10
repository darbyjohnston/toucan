// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ReadImageOpTest.h"

#include <toucan/ReadImageOp.h>

namespace toucan
{
    void readImageOpTest(const std::filesystem::path& path)
    {
        std::cout << "readImageOpTest" << std::endl;
        auto read = std::make_shared<ReadImageOp>();
        read->setPath(path / "Letter_A.png");
        auto buf = read->exec();
        const auto& spec = buf.spec();
        assert(spec.width > 0);
    }
}
