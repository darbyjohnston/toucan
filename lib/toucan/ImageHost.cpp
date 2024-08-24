// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageHost.h"

#include "Util.h"

#include <cstdarg>
#include <cstring>
#include <iostream>
#include <sstream>

namespace toucan
{
    ImageHost::ImageHost(
        const std::vector<std::filesystem::path>& searchPath,
        const ImageHostOptions& options) :
        _options(options)
    {
        _propSet.setPointer("host", 0, this);

        _host.host = (OfxPropertySetHandle)&_propSet;
        _host.fetchSuite = &_fetchSuite;

        _suiteInit();
        _pluginInit(searchPath);
    }

    ImageHost::~ImageHost()
    {
        for (const auto& data : _pluginData)
        {
            OfxStatus ofxStatus = data.ofxPlugin->mainEntry(
                kOfxActionUnload,
                nullptr,
                nullptr,
                nullptr);
        }
    }

    void ImageHost::generator(
        const std::string& name,
        OIIO::ImageBuf& output,
        const PropertySet& propSet)
    {
        for (auto& data : _pluginData)
        {
            if (name == data.ofxPlugin->pluginIdentifier)
            {
                OfxStatus ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxActionCreateInstance,
                    &data,
                    nullptr,
                    nullptr);

                data.instance.images["Output"] = bufToPropSet(output);
                PropertySet args = propSet;
                args.setDouble(kOfxPropTime, 0, 0.0);
                const auto& spec = output.spec();
                OfxRectI bounds;
                bounds.x1 = 0;
                bounds.x2 = spec.width;
                bounds.y1 = 0;
                bounds.y2 = spec.height;
                args.setIntN(kOfxImageEffectPropRenderWindow, 4, &bounds.x1);
                ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxImageEffectActionRender,
                    &data,
                    (OfxPropertySetHandle)&args,
                    nullptr);

                ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxActionDestroyInstance,
                    &data,
                    nullptr,
                    nullptr);
                break;
            }
        }
    }

    void ImageHost::filter(
        const std::string& name,
        const OIIO::ImageBuf& source,
        OIIO::ImageBuf& output,
        const PropertySet& propSet)
    {
        for (auto& data : _pluginData)
        {
            if (name == data.ofxPlugin->pluginIdentifier)
            {
                OfxStatus ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxActionCreateInstance,
                    &data,
                    nullptr,
                    nullptr);

                data.instance.images["Source"] = bufToPropSet(source);
                data.instance.images["Output"] = bufToPropSet(output);
                PropertySet args = propSet;
                args.setDouble(kOfxPropTime, 0, 0.0);
                const auto& spec = source.spec();
                OfxRectI bounds;
                bounds.x1 = 0;
                bounds.x2 = spec.width;
                bounds.y1 = 0;
                bounds.y2 = spec.height;
                args.setIntN(kOfxImageEffectPropRenderWindow, 4, &bounds.x1);
                ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxImageEffectActionRender,
                    &data,
                    (OfxPropertySetHandle)&args,
                    nullptr);

                ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxActionDestroyInstance,
                    &data,
                    nullptr,
                    nullptr);
                break;
            }
        }
    }

    void ImageHost::transition(
        const std::string& name,
        const OIIO::ImageBuf& sourceFrom,
        const OIIO::ImageBuf& sourceTo,
        OIIO::ImageBuf& output,
        const PropertySet& propSet)
    {
        for (auto& data : _pluginData)
        {
            if (name == data.ofxPlugin->pluginIdentifier)
            {
                OfxStatus ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxActionCreateInstance,
                    &data,
                    nullptr,
                    nullptr);

                data.instance.images["SourceFrom"] = bufToPropSet(sourceFrom);
                data.instance.images["SourceTo"] = bufToPropSet(sourceTo);
                data.instance.images["Output"] = bufToPropSet(output);
                PropertySet args = propSet;
                args.setDouble(kOfxPropTime, 0, 0.0);
                const auto& spec = sourceFrom.spec();
                OfxRectI bounds;
                bounds.x1 = 0;
                bounds.x2 = spec.width;
                bounds.y1 = 0;
                bounds.y2 = spec.height;
                args.setIntN(kOfxImageEffectPropRenderWindow, 4, &bounds.x1);
                ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxImageEffectActionRender,
                    &data,
                    (OfxPropertySetHandle)&args,
                    nullptr);

                ofxStatus = data.ofxPlugin->mainEntry(
                    kOfxActionDestroyInstance,
                    &data,
                    nullptr,
                    nullptr);
                break;
            }
        }
    }

    void ImageHost::_suiteInit()
    {
        _propertySuite.propSetPointer = &PropertySet::setPointer;
        _propertySuite.propSetString = &PropertySet::setString;
        _propertySuite.propSetDouble = &PropertySet::setDouble;
        _propertySuite.propSetInt = &PropertySet::setInt;
        _propertySuite.propSetPointerN = &PropertySet::setPointerN;
        _propertySuite.propSetStringN = &PropertySet::setStringN;
        _propertySuite.propSetDoubleN = &PropertySet::setDoubleN;
        _propertySuite.propSetIntN = &PropertySet::setIntN;
        _propertySuite.propGetPointer = &PropertySet::getPointer;
        _propertySuite.propGetString = &PropertySet::getString;
        _propertySuite.propGetDouble = &PropertySet::getDouble;
        _propertySuite.propGetInt = &PropertySet::getInt;
        _propertySuite.propGetPointerN = &PropertySet::getPointerN;
        _propertySuite.propGetStringN = &PropertySet::getStringN;
        _propertySuite.propGetDoubleN = &PropertySet::getDoubleN;
        _propertySuite.propGetIntN = &PropertySet::getIntN;
        _propertySuite.propReset = &PropertySet::reset;
        _propertySuite.propGetDimension = &PropertySet::getDimension;

        _parameterSuite.paramDefine = &_paramDefine;
        _parameterSuite.paramGetHandle = &_paramGetHandle;
        _parameterSuite.paramGetValue = &_paramGetValue;

        _effectSuite.getPropertySet = &_getPropertySet;
        _effectSuite.getParamSet = &_getParamSet;
        _effectSuite.clipDefine = &_clipDefine;
        _effectSuite.clipGetHandle = &_clipGetHandle;
        _effectSuite.clipGetImage = &_clipGetImage;
        _effectSuite.clipReleaseImage = &_clipReleaseImage;
    }

    void ImageHost::_pluginInit(const std::vector<std::filesystem::path>& searchPath)
    {
        // Find the plugins.
        if (_options.verbose)
        {
            std::cout << "Finding plugins..." << std::endl;
        }
        std::vector<std::filesystem::path> pluginPaths;
        for (const auto& path : searchPath)
        {
            if (_options.verbose)
            {
                std::cout << "  Search path: " << path.string() << std::endl;
            }
            findPlugins(path, pluginPaths);
        }

        // Load plugins.
        if (_options.verbose)
        {
            std::cout << "Loading plugins..." << std::endl;
        }
        for (const auto& path : pluginPaths)
        {
            if (_options.verbose)
            {
                std::cout << "  Path: " << path.string() << std::endl;
            }
            try
            {
                auto plugin = std::make_shared<Plugin>(path);
                const int pluginCount = plugin->getCount();
                for (int i = 0; i < pluginCount; ++i)
                {
                    OfxPlugin* ofxPlugin = plugin->getPlugin(i);
                    if (strcmp(ofxPlugin->pluginApi, kOfxImageEffectPluginApi) == 0)
                    {
                        if (_options.verbose)
                        {
                            std::cout << "    Plugin: " << ofxPlugin->pluginIdentifier << std::endl;
                        }
                        ofxPlugin->setHost(&_host);
                        OfxStatus ofxStatus = ofxPlugin->mainEntry(
                            kOfxActionLoad,
                            nullptr,
                            nullptr,
                            nullptr);
                        switch (ofxStatus)
                        {
                        case kOfxStatOK:
                        case kOfxStatReplyDefault:
                            _pluginData.push_back({ plugin, ofxPlugin });
                            break;
                        case kOfxStatErrFatal:
                        {
                            std::stringstream ss;
                            ss << "Fatal error in plugin: " << path.string();
                            throw std::runtime_error(ss.str());
                            break;
                        }
                        case kOfxStatFailed:
                        default:
                            break;
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cout << "ERROR: " << e.what() << std::endl;
            }
        }

        // Initialize plugins.
        if (_options.verbose)
        {
            std::cout << "Initializing plugins..." << std::endl;
        }
        for (auto& data : _pluginData)
        {
            if (_options.verbose)
            {
                std::cout << "  Plugin: " << data.ofxPlugin->pluginIdentifier << std::endl;
            }
            OfxStatus ofxStatus = data.ofxPlugin->mainEntry(
                kOfxActionDescribe,
                &data,
                nullptr,
                nullptr);
            int contextCount = 0;
            data.effectPropSet.getDimension(kOfxImageEffectPropSupportedContexts, &contextCount);
            for (int i = 0; i < contextCount; ++i)
            {
                char* context = nullptr;
                data.effectPropSet.getString(kOfxImageEffectPropSupportedContexts, i, &context);
                if (context)
                {
                    PropertySet propSet;
                    propSet.setString(kOfxImageEffectPropContext, 0, context);
                    if (_options.verbose)
                    {
                        std::cout << "    Context: " << context << std::endl;
                    }
                    ofxStatus = data.ofxPlugin->mainEntry(
                        kOfxImageEffectActionDescribeInContext,
                        &data,
                        (OfxPropertySetHandle)&propSet,
                        nullptr);

                    for (const auto& param : data.params)
                    {
                        auto props = param.second.getStringProperties();
                        auto i = std::find(props.begin(), props.end(), kOfxParamPropDefault);
                        if (i != props.end())
                        {
                            char* s = nullptr;
                            param.second.getString(kOfxParamPropDefault, 0, &s);
                            if (s)
                            {
                                data.instance.paramValues[param.first] = std::string(s);
                            }
                        }
                        props = param.second.getDoubleProperties();
                        i = std::find(props.begin(), props.end(), kOfxParamPropDefault);
                        if (i != props.end())
                        {
                            double d = 0.0;
                            param.second.getDouble(kOfxParamPropDefault, 0, &d);
                            data.instance.paramValues[param.first] = d;
                        }
                        props = param.second.getIntProperties();
                        i = std::find(props.begin(), props.end(), kOfxParamPropDefault);
                        if (i != props.end())
                        {
                            int i = 0;
                            param.second.getInt(kOfxParamPropDefault, 0, &i);
                            data.instance.paramValues[param.first] = i;
                        }
                    }
                }
            }
            if (_options.verbose)
            {
                for (const auto& param : data.paramTypes)
                {
                    std::cout << "    Parameter \"" << param.first << "\": " << param.second << std::endl;
                    for (const auto& prop : data.params[param.first].getStringProperties())
                    {
                        char* s = nullptr;
                        data.params[param.first].getString(prop.c_str(), 0, &s);
                        if (s)
                        {
                            std::cout << "      " << prop << ": " << s << std::endl;
                        }
                    }
                }
            }
        }
    }

    const void* ImageHost::_fetchSuite(OfxPropertySetHandle handle, const char* suiteName, int suiteVersion)
    {
        const void* out = nullptr;
        
        PropertySet* hostPropSet = reinterpret_cast<PropertySet*>(handle);
        void* hostP = nullptr;
        hostPropSet->getPointer("host", 0, &hostP);
        ImageHost* host = reinterpret_cast<ImageHost*>(hostP);

        if (strcmp(suiteName, kOfxPropertySuite) == 0)
        {
            out = &host->_propertySuite;
        }
        else if (strcmp(suiteName, kOfxParameterSuite) == 0)
        {
            out = &host->_parameterSuite;
        }
        else if (strcmp(suiteName, kOfxImageEffectSuite) == 0)
        {
            out = &host->_effectSuite;
        }
        return out;
    }
    
    OfxStatus ImageHost::_getPropertySet(OfxImageEffectHandle handle, OfxPropertySetHandle* propHandle)
    {
        PluginData* data = reinterpret_cast<PluginData*>(handle);
        *propHandle = (OfxPropertySetHandle)&data->effectPropSet;
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_getParamSet(OfxImageEffectHandle handle, OfxParamSetHandle* paramHandle)
    {
        *paramHandle = (OfxParamSetHandle)handle;
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_paramDefine(OfxParamSetHandle handle, const char* paramType, const char* name, OfxPropertySetHandle* propHandle)
    {
        PluginData* data = reinterpret_cast<PluginData*>(handle);
        data->paramTypes[name] = paramType;
        *propHandle = (OfxPropertySetHandle)&data->params[name];
        if (strcmp(paramType, kOfxParamTypeInteger) == 0)
        {
            data->instance.paramValues[name] = std::any(int());
        }
        if (strcmp(paramType, kOfxParamTypeDouble) == 0)
        {
            data->instance.paramValues[name] = std::any(double());
        }
        if (strcmp(paramType, kOfxParamTypeString) == 0)
        {
            data->instance.paramValues[name] = std::any(std::string());
        }
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_paramGetHandle(OfxParamSetHandle handle, const char* name, OfxParamHandle* paramHandle, OfxPropertySetHandle* propHandle)
    {
        PluginData* data = reinterpret_cast<PluginData*>(handle);
        auto i = data->instance.paramValues.find(name);
        if (i != data->instance.paramValues.end())
        {
            *paramHandle = (OfxParamHandle)&data->instance.paramValues[name];
        }
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_paramGetValue(OfxParamHandle handle, ...)
    {
        va_list args;
        va_start(args, handle);
        std::any* a = (std::any*)handle;
        if (a->type() == typeid(int))
        {
            const int value = std::any_cast<int>(*a);
            *va_arg(args, int*) = value;
        }
        if (a->type() == typeid(double))
        {
            const double value = std::any_cast<double>(*a);
            *va_arg(args, double*) = value;
        }
        if (a->type() == typeid(std::string))
        {
            const std::string value = std::any_cast<std::string>(*a);
            *va_arg(args, std::string*) = value;
        }
        va_end(args);
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_clipDefine(OfxImageEffectHandle handle, const char* name, OfxPropertySetHandle* propHandle)
    {
        PluginData* data = reinterpret_cast<PluginData*>(handle);
        *propHandle = (OfxPropertySetHandle)&data->clipPropSets[name];
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_clipGetHandle(OfxImageEffectHandle handle, const char* name, OfxImageClipHandle* clip, OfxPropertySetHandle* propHandle)
    {
        PluginData* data = reinterpret_cast<PluginData*>(handle);
        *clip = (OfxImageClipHandle)&data->instance.images[name];
        if (propHandle)
        {
            *propHandle = (OfxPropertySetHandle)&data->clipPropSets[name];
        }
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_clipGetImage(OfxImageClipHandle handle, OfxTime, const OfxRectD*, OfxPropertySetHandle* propHandle)
    {
        *propHandle = (OfxPropertySetHandle)handle;
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_clipReleaseImage(OfxPropertySetHandle handle)
    {
        return kOfxStatOK;
    }
}
