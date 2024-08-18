// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Plugin.h"

#include <OpenFX/ofxImageEffect.h>

Plugin::Plugin(const std::string& group, const std::string& name) :
    _name(name),
    _group(group)
{}

Plugin::~Plugin()
{}

OfxStatus Plugin::_entryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    OfxStatus out = kOfxStatReplyDefault;
    OfxImageEffectHandle effectHandle = (OfxImageEffectHandle)handle;
    if (strcmp(action, kOfxActionLoad) == 0)
    {
        out = _loadAction();
    }
    else if (strcmp(action, kOfxActionUnload) == 0)
    {
        out = _unloadAction();
    }
    else if (strcmp(action, kOfxActionDescribe) == 0)
    {
        out = _describeAction(effectHandle);
    }
    else if (strcmp(action, kOfxImageEffectActionDescribeInContext) == 0)
    {
        out = _describeInContextAction(effectHandle, inArgs);
    }
    else if (strcmp(action, kOfxImageEffectActionRender) == 0)
    {
        out = _renderAction(effectHandle, inArgs, outArgs);
    }
    return out;
}

OfxStatus Plugin::_loadAction(void)
{
    _propertySuite = (OfxPropertySuiteV1*)_host->fetchSuite(
        _host->host,
        kOfxPropertySuite,
        1);
    _imageEffectSuite = (OfxImageEffectSuiteV1*)_host->fetchSuite(
        _host->host,
        kOfxImageEffectSuite,
        1);
    return kOfxStatOK;
}

OfxStatus Plugin::_unloadAction(void)
{
    return kOfxStatOK;
}

OfxStatus Plugin::_describeAction(OfxImageEffectHandle descriptor)
{
    OfxPropertySetHandle effectProps;
    _imageEffectSuite->getPropertySet(descriptor, &effectProps);
    _propertySuite->propSetString(
        effectProps,
        kOfxPropLabel,
        0,
        _name.c_str());
    _propertySuite->propSetString(
        effectProps,
        kOfxImageEffectPluginPropGrouping,
        0,
        _group.c_str());
    return kOfxStatOK;
}

OfxStatus Plugin::_describeInContextAction(OfxImageEffectHandle descriptor, OfxPropertySetHandle inArgs)
{
    return kOfxStatOK;
}

OfxStatus Plugin::_renderAction(
    OfxImageEffectHandle instance,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return kOfxStatOK;
}
