// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "ImageEffect.h"

namespace toucan
{
    struct ImageEffectPlugin
    {
        std::shared_ptr<Plugin> plugin;
        OfxPlugin* ofxPlugin = nullptr;
        PropertySet propSet;
        std::map<std::string, PropertySet> clipPropSets;
        std::map<std::string, std::string> paramTypes;
        std::map<std::string, PropertySet> paramDefs;
    };

    struct ImageEffectInstance
    {
        std::map<std::string, std::any> params;
        std::map<std::string, PropertySet> images;
    };
}
