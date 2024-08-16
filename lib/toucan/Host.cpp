// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Host.h"

#include "Util.h"

#include <iostream>
#include <sstream>

namespace toucan
{
    Host::Host(const std::vector<std::filesystem::path>& searchPath)
    {
        _propertySet.setPointer("host", 0, this);

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

        _host.host = reinterpret_cast<OfxPropertySetHandle>(&_propertySet);
        _host.fetchSuite = &_fetchSuite;

        std::vector<std::filesystem::path> pluginPaths;
        for (const auto& path : searchPath)
        {
            findPlugins(path, pluginPaths);
        }
        for (auto const& path : pluginPaths)
        {
            //std::cout << path.string() << std::endl;
            try
            {
                auto plugin = std::make_shared<Plugin>(path);
                const int pluginCount = plugin->getCount();
                //std::cout << "plugin count: " << pluginCount << std::endl;
                if (pluginCount > 0)
                {
                    OfxPlugin* ofxPlugin = plugin->getPlugin(0);
                    //std::cout << "  plugin: " << ofxPlugin->pluginIdentifier << std::endl;
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
                        _plugins.push_back(plugin);
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
            catch (const std::exception& e)
            {
                std::cout << "ERROR: " << e.what() << std::endl;
            }
        }

        for (const auto& plugin : _plugins)
        {
            OfxStatus ofxStatus = plugin->getPlugin(0)->mainEntry(
                kOfxActionDescribe,
                plugin.get(),
                nullptr,
                nullptr);
            char* s = nullptr;
            plugin->getPropertySet(0)->getString(kOfxPropLabel, 0, &s);
            std::cout << "plugin: " << s << std::endl;
            plugin->getPropertySet(0)->getString(kOfxImageEffectPluginPropGrouping, 0, &s);
            std::cout << "    group: " << s << std::endl;
            plugin->getPropertySet(0)->getString(kOfxImageEffectPropSupportedContexts, 0, &s);
            std::cout << "    context: " << s << std::endl;
        }
    }

    Host::~Host()
    {
        for (const auto& plugin : _plugins)
        {
            OfxStatus ofxStatus = plugin->getPlugin(0)->mainEntry(
                kOfxActionUnload,
                nullptr,
                nullptr,
                nullptr);
        }
    }

    const void* Host::_fetchSuite(OfxPropertySetHandle handle, const char* suiteName, int suiteVersion)
    {
        const void* out = nullptr;
        
        PropertySet* hostPropertySet = reinterpret_cast<PropertySet*>(handle);
        void* hostP = nullptr;
        hostPropertySet->getPointer("host", 0, &hostP);
        Host* host = reinterpret_cast<Host*>(hostP);

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
    
    OfxStatus Host::_getPropertySet(OfxImageEffectHandle handle, OfxPropertySetHandle* propHandle)
    {
        Plugin* plugin = reinterpret_cast<Plugin*>(handle);
        *propHandle = reinterpret_cast<OfxPropertySetHandle>(plugin->getPropertySet(0));
        return kOfxStatOK;
    }
}
