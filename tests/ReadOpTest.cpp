// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ReadOpTest.h"

#include <toucan/ReadOp.h>

#include <cassert>

namespace toucan
{
    void readOpTest(
        const std::filesystem::path& path,
        const std::shared_ptr<Host>& host)
    {
        std::cout << "readOpTest" << std::endl;
        auto read = std::make_shared<ReadOp>(path / "Letter_A.png");
        auto buf = read->exec(OTIO_NS::RationalTime(), host);
        const auto& spec = buf.spec();
        assert(spec.width > 0);
    }
}
