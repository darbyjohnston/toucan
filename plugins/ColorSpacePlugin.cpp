// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ColorSpacePlugin.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

ColorSpacePlugin::ColorSpacePlugin(const std::string& group, const std::string& name) :
    Plugin(group, name)
{}

ColorSpacePlugin::~ColorSpacePlugin()
{}

OfxStatus ColorSpacePlugin::_describeAction(OfxImageEffectHandle descriptor)
{
    Plugin::_describeAction(descriptor);
    OfxPropertySetHandle effectProps;
    _imageEffectSuite->getPropertySet(descriptor, &effectProps);
    _propertySuite->propSetString(
        effectProps,
        kOfxImageEffectPropSupportedContexts,
        0,
        kOfxImageEffectContextFilter);
    return kOfxStatOK;
}

OfxStatus ColorSpacePlugin::_describeInContextAction(OfxImageEffectHandle descriptor, OfxPropertySetHandle inArgs)
{
    OfxPropertySetHandle sourceProps;
    OfxPropertySetHandle outputProps;
    _imageEffectSuite->clipDefine(descriptor, "Source", &sourceProps);
    _imageEffectSuite->clipDefine(descriptor, "Output", &outputProps);
    const std::vector<std::string> components =
    {
        kOfxImageComponentAlpha,
        kOfxImageComponentRGB,
        kOfxImageComponentRGBA
    };
    const std::vector<std::string> pixelDepths =
    {
        kOfxBitDepthByte,
        kOfxBitDepthShort,
        kOfxBitDepthFloat
    };
    for (int i = 0; i < components.size(); ++i)
    {
        _propertySuite->propSetString(
            sourceProps,
            kOfxImageEffectPropSupportedComponents,
            i,
            components[i].c_str());
        _propertySuite->propSetString(
            outputProps,
            kOfxImageEffectPropSupportedComponents,
            i,
            components[i].c_str());
    }
    for (int i = 0; i < pixelDepths.size(); ++i)
    {
        _propertySuite->propSetString(
            sourceProps,
            kOfxImageEffectPropSupportedPixelDepths,
            i,
            pixelDepths[i].c_str());
        _propertySuite->propSetString(
            outputProps,
            kOfxImageEffectPropSupportedPixelDepths,
            i,
            pixelDepths[i].c_str());
    }
    return kOfxStatOK;
}

OfxStatus ColorSpacePlugin::_renderAction(
    OfxImageEffectHandle instance,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    OfxTime time;
    OfxRectI renderWindow;
    _propertySuite->propGetDouble(inArgs, kOfxPropTime, 0, &time);
    _propertySuite->propGetIntN(inArgs, kOfxImageEffectPropRenderWindow, 4, &renderWindow.x1);

    OfxImageClipHandle sourceClip = nullptr;
    OfxImageClipHandle outputClip = nullptr;
    OfxPropertySetHandle sourceImage = nullptr;
    OfxPropertySetHandle outputImage = nullptr;
    _imageEffectSuite->clipGetHandle(instance, "Source", &sourceClip, nullptr);
    _imageEffectSuite->clipGetHandle(instance, "Output", &outputClip, nullptr);
    if (sourceClip && outputClip)
    {
        _imageEffectSuite->clipGetImage(sourceClip, time, nullptr, &sourceImage);
        _imageEffectSuite->clipGetImage(outputClip, time, nullptr, &outputImage);
        if (sourceImage && outputImage)
        {
            const OIIO::ImageBuf sourceBuf = propSetToBuf(_propertySuite, sourceImage);
            OIIO::ImageBuf outputBuf = propSetToBuf(_propertySuite, outputImage);
            _render(sourceBuf, outputBuf, renderWindow, inArgs);
        }
    }

    if (sourceImage)
    {
        _imageEffectSuite->clipReleaseImage(sourceImage);
    }
    if (outputImage)
    {
        _imageEffectSuite->clipReleaseImage(outputImage);
    }
    return kOfxStatOK;
}

PremultPlugin* PremultPlugin::_instance = nullptr;

PremultPlugin::PremultPlugin() :
    ColorSpacePlugin("Toucan", "Premult")
{}

PremultPlugin::~PremultPlugin()
{}

void PremultPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new PremultPlugin;
    }
    _instance->_host = host;
}

OfxStatus PremultPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus PremultPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    OIIO::ImageBufAlgo::premult(outputBuf, sourceBuf);
    return kOfxStatOK;
}

UnpremultPlugin* UnpremultPlugin::_instance = nullptr;

UnpremultPlugin::UnpremultPlugin() :
    ColorSpacePlugin("Toucan", "Unpremult")
{}

UnpremultPlugin::~UnpremultPlugin()
{}

void UnpremultPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new UnpremultPlugin;
    }
    _instance->_host = host;
}

OfxStatus UnpremultPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus UnpremultPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    OIIO::ImageBufAlgo::unpremult(outputBuf, sourceBuf);
    return kOfxStatOK;
}

namespace
{
    std::vector<OfxPlugin> plugins =
    {
        { kOfxImageEffectPluginApi, 1, "Toucan:Premult", 1, 0, PremultPlugin::setHostFunc, PremultPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Unpremult", 1, 0, UnpremultPlugin::setHostFunc, UnpremultPlugin::mainEntryPoint }
    };
}

extern "C"
{
    int OfxGetNumberOfPlugins(void)
    {
        return plugins.size();
    }

    OfxPlugin* OfxGetPlugin(int index)
    {
        if (index >= 0 && index < plugins.size())
        {
            return &plugins[index];
        }
        return 0;
    }
}
