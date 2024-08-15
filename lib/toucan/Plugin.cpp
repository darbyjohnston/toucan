// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Plugin.h"

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
        typedef int(__cdecl* GetNumberOfPluginsFnc)(void);
        typedef OfxPlugin* (__cdecl* OfxGetPluginFnc)(int);
    }

    struct Plugin::Private
    {
        HINSTANCE pluginInstance = NULL;
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

        GetNumberOfPluginsFnc getNumberOfPluginsFunc = (GetNumberOfPluginsFnc)GetProcAddress(
            _p->pluginInstance,
            "OfxGetNumberOfPlugins");
        if (!getNumberOfPluginsFunc)
        {
            std::stringstream ss;
            ss << "Cannot get number of plugins";
            throw std::runtime_error(ss.str());
        }
        std::cout << "Number of plugins: " << getNumberOfPluginsFunc() << std::endl;
    }

    Plugin::~Plugin()
    {
        FreeLibrary(_p->pluginInstance);
    }
}
