// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Gap.h"

namespace toucan
{
    Gap::Gap(
        const std::string& name,
        const OTIO_NS::TimeRange& range,
        const OTIO_NS::AnyDictionary& metadata) :
        IItem(name, range, metadata)
    {}

    Gap::~Gap()
    {}
}
