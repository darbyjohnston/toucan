// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/Plugin.h>
#include <toucan/PropertySet.h>

#include <OpenFX/ofxImageEffect.h>

#include <OpenImageIO/imagebuf.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    class Host : public std::enable_shared_from_this<Host>
    {
    public:
        Host(const std::vector<std::filesystem::path>& searchPath);

        ~Host();

        void filter(
            const std::string& name,
            const OIIO::ImageBuf&,
            OIIO::ImageBuf&,
            const PropertySet& = PropertySet());

    private:
        void _suiteInit();
        void _pluginInit(const std::vector<std::filesystem::path>& searchPath);

        static const void* _fetchSuite(OfxPropertySetHandle host, const char* suiteName, int suiteVersion);
        static OfxStatus _getPropertySet(OfxImageEffectHandle, OfxPropertySetHandle*);
        static OfxStatus _clipDefine(OfxImageEffectHandle, const char* name, OfxPropertySetHandle*);
        static OfxStatus _clipGetHandle(OfxImageEffectHandle, const char* name, OfxImageClipHandle*, OfxPropertySetHandle*);
        static OfxStatus _clipGetImage(OfxImageClipHandle, OfxTime, const OfxRectD*, OfxPropertySetHandle*);
        static OfxStatus _clipReleaseImage(OfxPropertySetHandle);

        PropertySet _propertySet;
        OfxHost _host;
        OfxPropertySuiteV1 _propertySuite;
        OfxImageEffectSuiteV1 _effectSuite;
        struct PluginData
        {
            std::shared_ptr<Plugin> plugin;
            OfxPlugin* ofxPlugin = nullptr;
            PropertySet effectPropertySet;
            std::map<std::string, PropertySet> clipPropertySets;
            std::map<std::string, PropertySet> images;
        };
        std::vector<PluginData> _pluginData;
    };
}
