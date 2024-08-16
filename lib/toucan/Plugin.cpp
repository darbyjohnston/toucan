// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Plugin.h"

#include "Util.h"

#include <OpenFX/ofxCore.h>

#include <iostream>
#include <sstream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>

namespace toucan
{
    namespace
    {
        typedef int(__cdecl* GetNumberOfPluginsFunc)(void);
        typedef OfxPlugin* (__cdecl* GetPluginFunc)(int);
        typedef void(__cdecl* SetHostFunc)(OfxHost*);
        typedef OfxStatus (__cdecl* MainEntryPointFunc)(
            const char*,
            const void*,
            OfxPropertySetHandle,
            OfxPropertySetHandle);
    }

    struct Plugin::Private
    {
        HINSTANCE pluginInstance = nullptr;
        GetNumberOfPluginsFunc getNumberOfPlugins = nullptr;
        GetPluginFunc getPlugin = nullptr;
    };

    Plugin::Plugin(const std::filesystem::path& path) :
        _p(new Private)
    {
        _p->pluginInstance = LoadLibrary(path.string().c_str());
        if (!_p->pluginInstance)
        {
            std::stringstream ss;
            ss << "Cannot load library: " << path.string();
            throw std::runtime_error(ss.str());
        }

        _p->getNumberOfPlugins = (GetNumberOfPluginsFunc)GetProcAddress(
            _p->pluginInstance,
            "OfxGetNumberOfPlugins");
        _p->getPlugin = (GetPluginFunc)GetProcAddress(
            _p->pluginInstance,
            "OfxGetPlugin");

        if (_p->getNumberOfPlugins)
        {
            _effectPropertySets.resize(_p->getNumberOfPlugins());
        }
    }

    Plugin::~Plugin()
    {
        FreeLibrary(_p->pluginInstance);
    }

    int Plugin::getCount()
    {
        return _p->getNumberOfPlugins ? _p->getNumberOfPlugins() : 0;
    }

    OfxPlugin* Plugin::getPlugin(int index)
    {
        return _p->getPlugin ? _p->getPlugin(index) : nullptr;
    }

    PropertySet* Plugin::getPropertySet(int index)
    {
        return index >= 0 && index  < _effectPropertySets.size() ?
            &_effectPropertySets[index] :
            nullptr;
    }
}
