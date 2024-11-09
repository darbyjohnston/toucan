// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucan/PropertySet.h>

#include <OpenFX/ofxCore.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    class Plugin : std::enable_shared_from_this<Plugin>
    {
    public:
        Plugin(const std::filesystem::path&);

        ~Plugin();

        int getCount();

        OfxPlugin* getPlugin(int);

    private:
        typedef int (*GetNumberOfPluginsFunc)(void);
        GetNumberOfPluginsFunc _getNumberOfPlugins = nullptr;

        typedef OfxPlugin* (*GetPluginFunc)(int);
        GetPluginFunc _getPlugin = nullptr;

        struct Private;
        std::unique_ptr<Private> _p;
    };
}
