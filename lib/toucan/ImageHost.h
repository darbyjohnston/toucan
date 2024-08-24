// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>
#include <toucan/Plugin.h>
#include <toucan/PropertySet.h>

#include <OpenFX/ofxImageEffect.h>

#include <OpenImageIO/imagebuf.h>

#include <opentimelineio/anyDictionary.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    //! Image effect plugin.
    struct ImageEffectPlugin
    {
        std::shared_ptr<Plugin> plugin;
        OfxPlugin* ofxPlugin = nullptr;
        PropertySet propSet;
        std::map<std::string, PropertySet> clipPropSets;
        std::map<std::string, std::string> paramTypes;
        std::map<std::string, PropertySet> paramDefs;
    };

    //! Image effect instance.
    struct ImageEffectInstance
    {
        std::map<std::string, std::any> params;
        std::map<std::string, PropertySet> images;
    };

    //! Image effect handle.
    struct ImageEffectHandle
    {
        ImageEffectPlugin* plugin = nullptr;
        ImageEffectInstance* instance = nullptr;
    };

    //! Image effect node.
    class ImageEffectNode : public IImageNode
    {
    public:
        ImageEffectNode(
            ImageEffectPlugin&,
            const std::string& name,
            const OTIO_NS::AnyDictionary& metaData,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~ImageEffectNode();

        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        ImageEffectPlugin& _plugin;
        ImageEffectInstance _instance;
        ImageEffectHandle _handle;
        OTIO_NS::AnyDictionary _metaData;
    };

    //! Image effect host options.
    struct ImageHostOptions
    {
        bool verbose = false;
    };

    //! Image effect host.
    class ImageHost : public std::enable_shared_from_this<ImageHost>
    {
    public:
        ImageHost(
            const std::vector<std::filesystem::path>& searchPath,
            const ImageHostOptions& = ImageHostOptions());

        ~ImageHost();

        //! Create an image node.
        std::shared_ptr<IImageNode> createNode(
            const std::string& name,
            const OTIO_NS::AnyDictionary&,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

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
        std::vector<ImageEffectPlugin> _plugins;
    };
}
