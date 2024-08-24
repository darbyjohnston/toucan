// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/Plugin.h>
#include <toucan/PropertySet.h>

#include <OpenFX/ofxImageEffect.h>

#include <OpenImageIO/imagebuf.h>

#include <any>
#include <filesystem>
#include <memory>

namespace toucan
{
    //! Image host options.
    struct ImageHostOptions
    {
        bool verbose = false;
    };

    //! Image plugins host.
    class ImageHost : public std::enable_shared_from_this<ImageHost>
    {
    public:
        ImageHost(
            const std::vector<std::filesystem::path>& searchPath,
            const ImageHostOptions& = ImageHostOptions());

        ~ImageHost();

        //! Generate an image.
        void generator(
            const std::string& name,
            OIIO::ImageBuf&,
            const PropertySet& = PropertySet());

        //! Apply a filter.
        void filter(
            const std::string& name,
            const OIIO::ImageBuf&,
            OIIO::ImageBuf&,
            const PropertySet& = PropertySet());

        //! Apply a transition.
        void transition(
            const std::string& name,
            const OIIO::ImageBuf&,
            const OIIO::ImageBuf&,
            OIIO::ImageBuf&,
            const PropertySet& = PropertySet());

    private:
        void _suiteInit();
        void _pluginInit(const std::vector<std::filesystem::path>& searchPath);

        static const void* _fetchSuite(OfxPropertySetHandle, const char* suiteName, int suiteVersion);
        static OfxStatus _getPropertySet(OfxImageEffectHandle, OfxPropertySetHandle*);
        static OfxStatus _getParamSet(OfxImageEffectHandle, OfxParamSetHandle*);
        static OfxStatus _paramDefine(OfxParamSetHandle, const char* paramType, const char* name, OfxPropertySetHandle*);
        static OfxStatus _paramGetHandle(OfxParamSetHandle, const char* name, OfxParamHandle*, OfxPropertySetHandle*);
        static OfxStatus _paramGetValue(OfxParamHandle, ...);
        static OfxStatus _clipDefine(OfxImageEffectHandle, const char* name, OfxPropertySetHandle*);
        static OfxStatus _clipGetHandle(OfxImageEffectHandle, const char* name, OfxImageClipHandle*, OfxPropertySetHandle*);
        static OfxStatus _clipGetImage(OfxImageClipHandle, OfxTime, const OfxRectD*, OfxPropertySetHandle*);
        static OfxStatus _clipReleaseImage(OfxPropertySetHandle);

        ImageHostOptions _options;
        PropertySet _propSet;
        OfxHost _host;
        OfxPropertySuiteV1 _propertySuite;
        OfxParameterSuiteV1 _parameterSuite;
        OfxImageEffectSuiteV1 _effectSuite;
        struct PluginData
        {
            std::shared_ptr<Plugin> plugin;
            OfxPlugin* ofxPlugin = nullptr;
            PropertySet effectPropSet;
            std::map<std::string, PropertySet> clipPropSets;
            std::map<std::string, std::string> paramTypes;
            std::map<std::string, PropertySet> params;
            
            struct InstanceData
            {
                std::map<std::string, std::any> paramValues;
                std::map<std::string, PropertySet> images;
            };
            //! \todo Add multiple instances.
            InstanceData instance;
        };
        std::vector<PluginData> _pluginData;
    };
}
