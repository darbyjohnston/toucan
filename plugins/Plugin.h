// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

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

    //! Get a parameter of an instance. Looked up on each use rather than
    //! kept from create-instance time: the plugin object is one per
    //! library and its instances render on more than one thread, so it
    //! holds nothing per instance.
    OfxParamHandle _param(OfxImageEffectHandle, const char* name) const;
    virtual OfxStatus _renderAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

    std::string _name;
    std::string _group;
    OfxHost* _host = nullptr;
    OfxPropertySuiteV1* _propSuite = nullptr;
    OfxParameterSuiteV1* _paramSuite = nullptr;
    OfxImageEffectSuiteV1* _effectSuite = nullptr;
};