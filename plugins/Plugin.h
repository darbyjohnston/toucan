// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <OpenFX/ofxImageEffect.h>
#include <OpenFX/ofxParam.h>

#include <string>

class Plugin
{
public:
    Plugin(const std::string& group, const std::string& name);

    virtual ~Plugin() = 0;

protected:
    virtual OfxStatus _entryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);
    virtual OfxStatus _loadAction(void);
    virtual OfxStatus _unloadAction(void);
    virtual OfxStatus _describeAction(OfxImageEffectHandle);
    virtual OfxStatus _describeInContextAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle);
    virtual OfxStatus _createInstance(OfxImageEffectHandle);
    virtual OfxStatus _destroyInstance(OfxImageEffectHandle);
    virtual OfxStatus _renderAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

    std::string _name;
    std::string _group;
    OfxHost* _host = nullptr;
    OfxPropertySuiteV1* _propertySuite = nullptr;
    OfxParameterSuiteV1* _parameterSuite = nullptr;
    OfxImageEffectSuiteV1* _imageEffectSuite = nullptr;
};