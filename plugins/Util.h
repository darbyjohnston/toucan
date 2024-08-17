// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <OpenFX/ofxProperty.h>

#include <OpenImageIO/imagebuf.h>

//! Convert from a property set.
OIIO::ImageBuf propSetToBuf(OfxPropertySuiteV1*, OfxPropertySetHandle);
