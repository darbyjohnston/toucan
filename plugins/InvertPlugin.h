// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <OpenFX/ofxCore.h>

#include <memory>

extern "C"
{
    int OfxGetNumberOfPlugins(void);
    OfxPlugin* OfxGetPlugin(int index);
    void SetHostFunc(OfxHost*);
    OfxStatus MainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);
}
