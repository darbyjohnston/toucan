// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/Plugin.h>
#include <toucan/PropertySet.h>

#include <OpenFX/ofxImageEffect.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    class Host : public std::enable_shared_from_this<Host>
    {
    public:
        Host(const std::vector<std::filesystem::path>& searchPath);

        ~Host();

    private:
        static const void* _fetchSuite(OfxPropertySetHandle host, const char* suiteName, int suiteVersion);

        static OfxStatus _getPropertySet(OfxImageEffectHandle, OfxPropertySetHandle* propHandle);

        PropertySet _propertySet;
        OfxPropertySuiteV1 _propertySuite;
        OfxImageEffectSuiteV1 _effectSuite;
        OfxHost _host;
        std::vector<std::shared_ptr<Plugin> > _plugins;
    };
}
