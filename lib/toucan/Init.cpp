// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Init.h"

#include "Box.h"
#include "Checkers.h"
#include "ColorMap.h"
#include "Fill.h"
#include "Flip.h"
#include "Flop.h"
#include "Invert.h"
#include "Line.h"
#include "Noise.h"
#include "Pow.h"
#include "Resize.h"
#include "Rotate.h"
#include "Saturate.h"
#include "Text.h"

#include <opentimelineio/typeRegistry.h>

namespace toucan
{
    void init()
    {
        OTIO_NS::TypeRegistry::instance().register_type<BoxEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<CheckersEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<ColorMapEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FillEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FlipEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<FlopEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<InvertEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<LineEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<NoiseEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<PowEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<ResizeEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<RotateEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<SaturateEffect>();
        OTIO_NS::TypeRegistry::instance().register_type<TextEffect>();
    }
}
