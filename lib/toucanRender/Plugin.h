// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/PropertySet.h>

#include <OpenFX/ofxCore.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    //! Plugin.
    class Plugin : std::enable_shared_from_this<Plugin>
    {
    public:
        Plugin(const std::filesystem::path&);

        ~Plugin();

        //! Get the number of plugins.
        int getCount();

        //! Get a plugin.
        OfxPlugin* getPlugin(int);

    private:
        typedef int (*GetNumberOfPluginsFunc)(void);
        GetNumberOfPluginsFunc _getNumberOfPlugins = nullptr;

        typedef OfxPlugin* (*GetPluginFunc)(int);
        GetPluginFunc _getPlugin = nullptr;

        struct Private;
        std::unique_ptr<Private> _p;
    };

    //! Find plugins.
    void findPlugins(const std::filesystem::path&, std::vector<std::filesystem::path>&);
}
