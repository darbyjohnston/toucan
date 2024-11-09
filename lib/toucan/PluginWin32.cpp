// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "Plugin.h"

#include "Util.h"

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
    struct Plugin::Private
    {
        HINSTANCE pluginInstance = nullptr;
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

        _getNumberOfPlugins = (GetNumberOfPluginsFunc)GetProcAddress(
            _p->pluginInstance,
            "OfxGetNumberOfPlugins");
        _getPlugin = (GetPluginFunc)GetProcAddress(
            _p->pluginInstance,
            "OfxGetPlugin");
    }

    Plugin::~Plugin()
    {
        FreeLibrary(_p->pluginInstance);
    }
}
