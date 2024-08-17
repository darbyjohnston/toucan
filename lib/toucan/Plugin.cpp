// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

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
