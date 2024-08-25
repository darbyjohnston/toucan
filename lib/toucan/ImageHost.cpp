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
    ImageEffectNode::ImageEffectNode(
        ImageEffectPlugin& plugin,
        const std::string& name,
        const OTIO_NS::AnyDictionary& metaData,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(name, inputs),
        _plugin(plugin),
        _handle{ &plugin, &_instance },
        _metaData(metaData)
    {
        // Set default values.
        for (const auto& param : _plugin.paramDefs)
        {
            auto props = param.second.getStringProperties();
            auto i = std::find(props.begin(), props.end(), kOfxParamPropDefault);
            if (i != props.end())
            {
                char* s = nullptr;
                param.second.getString(kOfxParamPropDefault, 0, &s);
                if (s)
                {
                    _instance.params[param.first] = std::string(s);
                }
            }
            props = param.second.getDoubleProperties();
            i = std::find(props.begin(), props.end(), kOfxParamPropDefault);
            if (i != props.end())
            {
                double d = 0.0;
                param.second.getDouble(kOfxParamPropDefault, 0, &d);
                _instance.params[param.first] = d;
            }
            props = param.second.getIntProperties();
            i = std::find(props.begin(), props.end(), kOfxParamPropDefault);
            if (i != props.end())
            {
                int i = 0;
                param.second.getInt(kOfxParamPropDefault, 0, &i);
                _instance.params[param.first] = i;
            }
        }

        // Set values.
        for (const auto& i : metaData)
        {
            _instance.params[i.first] = i.second;
        }

        // Create the plugin instance.
        OfxStatus ofxStatus = _plugin.ofxPlugin->mainEntry(
            kOfxActionCreateInstance,
            &_handle,
            nullptr,
            nullptr);
    }

    ImageEffectNode::~ImageEffectNode()
    {
        // Destroy the plugin instance.
        OfxStatus ofxStatus = _plugin.ofxPlugin->mainEntry(
            kOfxActionDestroyInstance,
            &_handle,
            nullptr,
            nullptr);
    }

    OIIO::ImageBuf ImageEffectNode::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf out;

        OTIO_NS::RationalTime offsetTime = time;
        if (!_timeOffset.is_invalid_time())
        {
            offsetTime -= _timeOffset;
        }

        // Initialize the images.
        char* context = nullptr;
        _plugin.propSet.getString(kOfxImageEffectPropSupportedContexts, 0, &context);
        if (strcmp(context, kOfxImageEffectContextGenerator) == 0)
        {
            auto i = _metaData.find("size");
            if (i != _metaData.end() && i->second.has_value())
            {
                IMATH_NAMESPACE::V2i size = IMATH_NAMESPACE::V2i(0, 0);
                anyToVec(std::any_cast<OTIO_NS::AnyVector>(i->second), size);

                out = OIIO::ImageBuf(OIIO::ImageSpec(size.x, size.y, 4));
                _instance.images["Output"] = bufToPropSet(out);
            }
        }
        else if (
            strcmp(context, kOfxImageEffectContextFilter) == 0 &&
            !_inputs.empty() &&
            _inputs[0])
        {
            const auto input = _inputs[0]->exec(offsetTime);
            out = OIIO::ImageBuf(input.spec());
            _instance.images["Source"] = bufToPropSet(input);
            _instance.images["Output"] = bufToPropSet(out);
        }
        else if (
            strcmp(context, kOfxImageEffectContextTransition) == 0 &&
            _inputs.size() > 1 &&
            _inputs[0] &&
            _inputs[1])
        {
            const auto input0 = _inputs[0]->exec(offsetTime);
            const auto input1 = _inputs[1]->exec(offsetTime);
            out = OIIO::ImageBuf(input0.spec());
            _instance.images["SourceFrom"] = bufToPropSet(input0);
            _instance.images["SourceTo"] = bufToPropSet(input1);
            _instance.images["Output"] = bufToPropSet(out);
        }

        // Render.
        const auto& spec = out.spec();
        if (spec.width > 0 && spec.height > 0)
        {
            PropertySet args;
            args.setDouble(kOfxPropTime, 0, offsetTime.value());
            OfxRectI bounds;
            bounds.x1 = 0;
            bounds.x2 = spec.width;
            bounds.y1 = 0;
            bounds.y2 = spec.height;
            args.setIntN(kOfxImageEffectPropRenderWindow, 4, &bounds.x1);

            _plugin.ofxPlugin->mainEntry(
                kOfxImageEffectActionRender,
                &_handle,
                (OfxPropertySetHandle)&args,
                nullptr);
        }

        return out;
    }

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
        for (const auto& plugin : _plugins)
        {
            OfxStatus ofxStatus = plugin.ofxPlugin->mainEntry(
                kOfxActionUnload,
                nullptr,
                nullptr,
                nullptr);
        }
    }

    std::shared_ptr<IImageNode> ImageHost::createNode(
        const std::string& name,
        const OTIO_NS::AnyDictionary& metaData,
        const std::vector<std::shared_ptr<IImageNode> >& inputs)
    {
        std::shared_ptr<IImageNode> out;
        for (auto& plugin : _plugins)
        {
            if (name == plugin.ofxPlugin->pluginIdentifier)
            {
                out = std::make_shared<ImageEffectNode>(plugin, name, metaData, inputs);
                break;
            }
        }
        return out;
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

        // Load the plugins.
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
                            _plugins.push_back({ plugin, ofxPlugin });
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

        // Initialize the plugins.
        if (_options.verbose)
        {
            std::cout << "Initializing plugins..." << std::endl;
        }
        for (auto& plugin : _plugins)
        {
            if (_options.verbose)
            {
                std::cout << "  Plugin: " << plugin.ofxPlugin->pluginIdentifier << std::endl;
            }
            ImageEffectHandle handle = { &plugin };
            OfxStatus ofxStatus = plugin.ofxPlugin->mainEntry(
                kOfxActionDescribe,
                &handle,
                nullptr,
                nullptr);
            int contextCount = 0;
            plugin.propSet.getDimension(kOfxImageEffectPropSupportedContexts, &contextCount);
            for (int i = 0; i < contextCount; ++i)
            {
                char* context = nullptr;
                plugin.propSet.getString(kOfxImageEffectPropSupportedContexts, i, &context);
                if (context)
                {
                    PropertySet propSet;
                    propSet.setString(kOfxImageEffectPropContext, 0, context);
                    if (_options.verbose)
                    {
                        std::cout << "    Context: " << context << std::endl;
                    }
                    ofxStatus = plugin.ofxPlugin->mainEntry(
                        kOfxImageEffectActionDescribeInContext,
                        &handle,
                        (OfxPropertySetHandle)&propSet,
                        nullptr);
                }
            }
            if (_options.verbose)
            {
                for (const auto& param : plugin.paramTypes)
                {
                    std::cout << "    Parameter \"" << param.first << "\": " << param.second << std::endl;
                    for (const auto& prop : plugin.paramDefs[param.first].getStringProperties())
                    {
                        char* s = nullptr;
                        plugin.paramDefs[param.first].getString(prop.c_str(), 0, &s);
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
        
        PropertySet* hostPropSet = (PropertySet*)handle;
        ImageHost* host = nullptr;
        hostPropSet->getPointer("host", 0, (void**)&host);

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
    
    OfxStatus ImageHost::_getPropertySet(OfxImageEffectHandle effectHandle, OfxPropertySetHandle* propHandle)
    {
        ImageEffectHandle* handle = (ImageEffectHandle*)effectHandle;
        *propHandle = (OfxPropertySetHandle)&handle->plugin->propSet;
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_getParamSet(OfxImageEffectHandle effectHandle, OfxParamSetHandle* paramHandle)
    {
        *paramHandle = (OfxParamSetHandle)effectHandle;
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_paramDefine(OfxParamSetHandle effectHandle, const char* paramType, const char* name, OfxPropertySetHandle* propHandle)
    {
        ImageEffectHandle* handle = (ImageEffectHandle*)effectHandle;
        handle->plugin->paramTypes[name] = paramType;
        *propHandle = (OfxPropertySetHandle)&handle->plugin->paramDefs[name];
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_paramGetHandle(OfxParamSetHandle effectHandle, const char* name, OfxParamHandle* paramHandle, OfxPropertySetHandle* propHandle)
    {
        ImageEffectHandle* handle = (ImageEffectHandle*)effectHandle;
        auto i = handle->instance->params.find(name);
        if (i != handle->instance->params.end())
        {
            *paramHandle = (OfxParamHandle)&handle->instance->params[name];
        }
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_paramGetValue(OfxParamHandle handle, ...)
    {
        va_list args;
        va_start(args, handle);
        std::any* a = (std::any*)handle;
        if (a->type() == typeid(bool))
        {
            const bool value = std::any_cast<bool>(*a);
            *va_arg(args, bool*) = value;
        }
        else if (a->type() == typeid(int))
        {
            const int value = std::any_cast<int>(*a);
            *va_arg(args, int*) = value;
        }
        else if (a->type() == typeid(double))
        {
            const double value = std::any_cast<double>(*a);
            *va_arg(args, double*) = value;
        }
        else if (a->type() == typeid(std::string))
        {
            const std::string value = std::any_cast<std::string>(*a);
            *va_arg(args, std::string*) = value;
        }
        else if (a->type() == typeid(OTIO_NS::AnyVector))
        {
            const OTIO_NS::AnyVector value = std::any_cast<OTIO_NS::AnyVector>(*a);
            if (!value.empty() && value[0].type() == typeid(double))
            {
                for (size_t i = 0; i < value.size(); ++i)
                {
                    *va_arg(args, double*) = std::any_cast<double>(value[i]);
                }
            }
            else if (!value.empty() && value[0].type() == typeid(int64_t))
            {
                for (size_t i = 0; i < value.size(); ++i)
                {
                    *va_arg(args, int64_t*) = std::any_cast<int64_t>(value[i]);
                }
            }
        }
        va_end(args);
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_clipDefine(OfxImageEffectHandle effectHandle, const char* name, OfxPropertySetHandle* propHandle)
    {
        ImageEffectHandle* handle = (ImageEffectHandle*)effectHandle;
        *propHandle = (OfxPropertySetHandle)&handle->plugin->clipPropSets[name];
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_clipGetHandle(OfxImageEffectHandle effectHandle, const char* name, OfxImageClipHandle* clip, OfxPropertySetHandle* propHandle)
    {
        ImageEffectHandle* handle = (ImageEffectHandle*)effectHandle;
        *clip = (OfxImageClipHandle)&handle->instance->images[name];
        if (propHandle)
        {
            *propHandle = (OfxPropertySetHandle)&handle->plugin->clipPropSets[name];
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
