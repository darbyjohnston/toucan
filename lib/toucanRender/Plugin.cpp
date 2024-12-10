// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "Plugin.h"

namespace toucan
{
    int Plugin::getCount()
    {
        return _getNumberOfPlugins ? _getNumberOfPlugins() : 0;
    }

    OfxPlugin* Plugin::getPlugin(int index)
    {
        return _getPlugin ? _getPlugin(index) : nullptr;
    }
}
