// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageHost.h"

#include "Util.h"

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
        _propertySet.setPointer("host", 0, this);

        _host.host = (OfxPropertySetHandle)&_propertySet;
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

                data.images["Output"] = bufToPropSet(output);
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

                data.images["Source"] = bufToPropSet(source);
                data.images["Output"] = bufToPropSet(output);
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

                data.images["SourceFrom"] = bufToPropSet(sourceFrom);
                data.images["SourceTo"] = bufToPropSet(sourceTo);
                data.images["Output"] = bufToPropSet(output);
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

        _effectSuite.getPropertySet = &_getPropertySet;
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
                std::cout << "    Search path: " << path.string() << std::endl;
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
                std::cout << "    Plugin path: " << path.string() << std::endl;
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
                            std::cout << "        Plugin: " << ofxPlugin->pluginIdentifier << std::endl;
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
                std::cout << "    Plugin: " << data.ofxPlugin->pluginIdentifier << std::endl;
            }
            OfxStatus ofxStatus = data.ofxPlugin->mainEntry(
                kOfxActionDescribe,
                &data,
                nullptr,
                nullptr);
            int contextCount = 0;
            data.effectPropertySet.getDimension(kOfxImageEffectPropSupportedContexts, &contextCount);
            for (int i = 0; i < contextCount; ++i)
            {
                char* context = nullptr;
                data.effectPropertySet.getString(kOfxImageEffectPropSupportedContexts, i, &context);
                if (context)
                {
                    PropertySet propertySet;
                    propertySet.setString(kOfxImageEffectPropContext, 0, context);
                    if (_options.verbose)
                    {
                        std::cout << "        Context: " << context << std::endl;
                    }
                    ofxStatus = data.ofxPlugin->mainEntry(
                        kOfxImageEffectActionDescribeInContext,
                        &data,
                        (OfxPropertySetHandle)&propertySet,
                        nullptr);
                }
            }
            /*if (_options.verbose)
            {
                for (const auto& property : data.effectPropertySet.getPointerProperties())
                {
                    std::cout << "        Pointer property: " << property << std::endl;
                }
                for (const auto& property : data.effectPropertySet.getStringProperties())
                {
                    std::cout << "        String property: " << property << std::endl;
                }
                for (const auto& property : data.effectPropertySet.getDoubleProperties())
                {
                    std::cout << "        Double property: " << property << std::endl;
                }
                for (const auto& property : data.effectPropertySet.getIntProperties())
                {
                    std::cout << "        Int property: " << property << std::endl;
                }
            }*/
        }
    }

    const void* ImageHost::_fetchSuite(OfxPropertySetHandle handle, const char* suiteName, int suiteVersion)
    {
        const void* out = nullptr;
        
        PropertySet* hostPropertySet = reinterpret_cast<PropertySet*>(handle);
        void* hostP = nullptr;
        hostPropertySet->getPointer("host", 0, &hostP);
        ImageHost* host = reinterpret_cast<ImageHost*>(hostP);

        if (strcmp(suiteName, kOfxPropertySuite) == 0)
        {
            out = &host->_propertySuite;
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
        *propHandle = (OfxPropertySetHandle)&data->effectPropertySet;
        return kOfxStatOK;
    }
    
    OfxStatus ImageHost::_clipDefine(OfxImageEffectHandle handle, const char* name, OfxPropertySetHandle* propHandle)
    {
        PluginData* data = reinterpret_cast<PluginData*>(handle);
        *propHandle = (OfxPropertySetHandle)&data->clipPropertySets[name];
        return kOfxStatOK;
    }

    OfxStatus ImageHost::_clipGetHandle(OfxImageEffectHandle handle, const char* name, OfxImageClipHandle* clip, OfxPropertySetHandle* propHandle)
    {
        PluginData* data = reinterpret_cast<PluginData*>(handle);
        *clip = (OfxImageClipHandle)&data->images[name];
        if (propHandle)
        {
            *propHandle = (OfxPropertySetHandle)&data->clipPropertySets[name];
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