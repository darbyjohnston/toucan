// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "InvertPlugin.h"

#include <OpenFX/ofxImageEffect.h>

namespace toucan
{
    namespace
    {
        InvertPlugin* plugin = nullptr;

        OfxPlugin effectPluginStruct =
        {
            kOfxImageEffectPluginApi,
            1,
            "toucan:InvertPlugin",
            1,
            0,
            SetHostFunc,
            MainEntryPoint
        };
    }

    InvertPlugin::InvertPlugin(OfxHost* host) :
        _host(host)
    {}

    InvertPlugin::~InvertPlugin()
    {}
}

extern "C"
{
    int OfxGetNumberOfPlugins(void)
    {
        return 1;
    }

    OfxPlugin* OfxGetPlugin(int index)
    {
        if (0 == index)
        {
            return &toucan::effectPluginStruct;
        }
        return 0;
    }

    void SetHostFunc(OfxHost* value)
    {
        toucan::plugin = new toucan::InvertPlugin(value);
    }

    OfxStatus MainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs)
    {
        return kOfxStatReplyDefault;
    }
}