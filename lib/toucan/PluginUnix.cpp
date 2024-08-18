// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Plugin.h"

#include "Util.h"

#include <iostream>
#include <sstream>

#include <dlfcn.h>

namespace toucan
{
    struct Plugin::Private
    {
        void* plugin = nullptr;
    };

    Plugin::Plugin(const std::filesystem::path& path) :
        _p(new Private)
    {
        _p->plugin = dlopen(path.string().c_str(), RTLD_NOW|RTLD_GLOBAL);
        if (!_p->plugin)
        {
            std::stringstream ss;
            ss << "Cannot load library: " << path.string();
            throw std::runtime_error(ss.str());
        }

        _getNumberOfPlugins = (GetNumberOfPluginsFunc)dlsym(
            _p->plugin,
            "OfxGetNumberOfPlugins");
        _getPlugin = (GetPluginFunc)dlsym(
            _p->plugin,
            "OfxGetPlugin");
    }

    Plugin::~Plugin()
    {
        dlclose(_p->plugin);
    }
}
