// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Init.h"

#include "CheckersImageOp.h"
#include "FillImageOp.h"
#include "NoiseImageOp.h"
#include "TextImageOp.h"

#include <opentimelineio/typeRegistry.h>

namespace toucan
{
    void init()
    {
        OTIO_NS::TypeRegistry::instance().register_type<CheckersImageEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FillImageEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<NoiseImageEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<TextImageEffect>();
    }
}
