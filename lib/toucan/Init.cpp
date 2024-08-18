// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Init.h"

#include "Comp.h"
#include "Draw.h"
#include "Filter.h"
#include "Read.h"
#include "TimeWarp.h"
#include "Transform.h"
#include "Transition.h"

#include <opentimelineio/typeRegistry.h>

namespace toucan
{
    void init()
    {
        OTIO_NS::TypeRegistry::instance().register_type<BlurEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<BoxEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<ColorMapEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FlipEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FlopEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<InvertEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<LineEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<PowEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<ResizeEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<RotateEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<SaturateEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<TextEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<UnsharpMaskEffect>();
    }
}
