// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "InvertPlugin.h"

#include <OpenFX/ofxImageEffect.h>

#include <iostream>

namespace
{
    OfxHost* host = nullptr;
    OfxPropertySuiteV1* propertySuite = nullptr;
    OfxImageEffectSuiteV1* imageEffectSuite = nullptr;
    OfxPlugin effectPluginStruct =
    {
        kOfxImageEffectPluginApi,
        1,
        "toucan:invert",
        1,
        0,
        SetHostFunc,
        MainEntryPoint
    };
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
            return &effectPluginStruct;
        }
        return 0;
    }

    void SetHostFunc(OfxHost* value)
    {
        host = value;
    }

    OfxStatus LoadAction(void)
    {
        propertySuite = (OfxPropertySuiteV1*)host->fetchSuite(
            host->host,
            kOfxPropertySuite,
            1);
        imageEffectSuite = (OfxImageEffectSuiteV1*)host->fetchSuite(
            host->host,
            kOfxImageEffectSuite,
            1);
        return kOfxStatOK;
    }

    OfxStatus UnloadAction(void)
    {
        imageEffectSuite = nullptr;
        propertySuite = nullptr;
        host = nullptr;
        return kOfxStatOK;
    }

    OfxStatus DescribeAction(OfxImageEffectHandle descriptor)
    {
        OfxPropertySetHandle effectProps;
        imageEffectSuite->getPropertySet(descriptor, &effectProps);
        propertySuite->propSetString(
            effectProps,
            kOfxPropLabel,
            0,
            "invert");
        propertySuite->propSetString(
            effectProps,
            kOfxImageEffectPluginPropGrouping,
            0,
            "toucan");
        propertySuite->propSetString(
            effectProps,
            kOfxImageEffectPropSupportedContexts,
            0,
            kOfxImageEffectContextFilter);
        return kOfxStatOK;
    }

    OfxStatus MainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs)
    {
        OfxStatus out = kOfxStatReplyDefault;
        //std::cout << "MainEntryPoint: " << action << std::endl;
        if (strcmp(action, kOfxActionLoad) == 0)
        {
            out = LoadAction();
        }
        else if (strcmp(action, kOfxActionUnload) == 0)
        {
            out = UnloadAction();
        }
        else if (strcmp(action, kOfxActionDescribe) == 0)
        {
            out = DescribeAction((OfxImageEffectHandle)handle);
        }
        return out;
    }
}