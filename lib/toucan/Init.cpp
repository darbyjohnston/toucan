// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Init.h"

#include "BoxOp.h"
#include "CheckersOp.h"
#include "FillOp.h"
#include "FlipOp.h"
#include "FlopOp.h"
#include "InvertOp.h"
#include "LineOp.h"
#include "NoiseOp.h"
#include "ResizeOp.h"
#include "RotateOp.h"
#include "SaturateOp.h"
#include "TextOp.h"

#include <opentimelineio/typeRegistry.h>

namespace toucan
{
    void init()
    {
        OTIO_NS::TypeRegistry::instance().register_type<BoxEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<CheckersEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FillEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FlipEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FlopEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<InvertEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<LineEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<NoiseEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<ResizeEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<RotateEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<SaturateEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<TextEffect>();
    }
}
