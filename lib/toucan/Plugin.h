// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

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

        PropertySet* getPropertySet(int);

    private:
        std::vector<PropertySet> _effectPropertySets;
        struct Private;
        std::unique_ptr<Private> _p;
    };
}
