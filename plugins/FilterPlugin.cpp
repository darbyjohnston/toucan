// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "FilterPlugin.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

FilterPlugin::FilterPlugin(const std::string& group, const std::string& name) :
    Plugin(group, name)
{}

FilterPlugin::~FilterPlugin()
{}

OfxStatus FilterPlugin::_describeAction(OfxImageEffectHandle descriptor)
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

OfxStatus FilterPlugin::_describeInContextAction(OfxImageEffectHandle descriptor, OfxPropertySetHandle inArgs)
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

OfxStatus FilterPlugin::_renderAction(
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

ColorMapPlugin* ColorMapPlugin::_instance = nullptr;

ColorMapPlugin::ColorMapPlugin() :
    FilterPlugin("Toucan", "ColorMap")
{}

ColorMapPlugin::~ColorMapPlugin()
{}

void ColorMapPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new ColorMapPlugin;
    }
    _instance->_host = host;
}

OfxStatus ColorMapPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus ColorMapPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    // Apply the color map.
    std::string mapName = "plasma";
    char* s = nullptr;
    _propertySuite->propGetString(inArgs, "mapName", 0, &s);
    if (s)
    {
        mapName = s;
    }
    OIIO::ImageBufAlgo::color_map(
        outputBuf,
        sourceBuf,
        -1,
        mapName,
        OIIO::ROI(
            renderWindow.x1,
            renderWindow.x2,
            renderWindow.y1,
            renderWindow.y2));

    // Copy the alpha channel.
    OIIO::ImageBufAlgo::copy(
        outputBuf,
        sourceBuf,
        OIIO::TypeUnknown,
        OIIO::ROI(
            renderWindow.x1,
            renderWindow.x2,
            renderWindow.y1,
            renderWindow.y2,
            0,
            1,
            3,
            4));

    return kOfxStatOK;
}

InvertPlugin* InvertPlugin::_instance = nullptr;

InvertPlugin::InvertPlugin() :
    FilterPlugin("Toucan", "Invert")
{}

InvertPlugin::~InvertPlugin()
{}

void InvertPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new InvertPlugin;
    }
    _instance->_host = host;
}

OfxStatus InvertPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus InvertPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    // Invert the color channels.
    OIIO::ImageBufAlgo::invert(
        outputBuf,
        sourceBuf,
        OIIO::ROI(
            renderWindow.x1,
            renderWindow.x2,
            renderWindow.y1,
            renderWindow.y2,
            0,
            1,
            0,
            3));

    // Copy the alpha channel.
    OIIO::ImageBufAlgo::copy(
        outputBuf,
        sourceBuf,
        OIIO::TypeUnknown,
        OIIO::ROI(
            renderWindow.x1,
            renderWindow.x2,
            renderWindow.y1,
            renderWindow.y2,
            0,
            1,
            3,
            4));

    return kOfxStatOK;
}

PowPlugin* PowPlugin::_instance = nullptr;

PowPlugin::PowPlugin() :
    FilterPlugin("Toucan", "Pow")
{}

PowPlugin::~PowPlugin()
{}

void PowPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new PowPlugin;
    }
    _instance->_host = host;
}

OfxStatus PowPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus PowPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    double value = 1.0;
    _propertySuite->propGetDouble(inArgs, "value", 0, &value);

    OIIO::ImageBufAlgo::pow(
        outputBuf,
        sourceBuf,
        value,
        OIIO::ROI(
            renderWindow.x1,
            renderWindow.x2,
            renderWindow.y1,
            renderWindow.y2));
    
    return kOfxStatOK;
}

SaturatePlugin* SaturatePlugin::_instance = nullptr;

SaturatePlugin::SaturatePlugin() :
    FilterPlugin("Toucan", "Saturate")
{}

SaturatePlugin::~SaturatePlugin()
{}

void SaturatePlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new SaturatePlugin;
    }
    _instance->_host = host;
}

OfxStatus SaturatePlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus SaturatePlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    double value = 1.0;
    _propertySuite->propGetDouble(inArgs, "value", 0, &value);

    OIIO::ImageBufAlgo::saturate(
        outputBuf,
        sourceBuf,
        value,
        0,
        OIIO::ROI(
            renderWindow.x1,
            renderWindow.x2,
            renderWindow.y1,
            renderWindow.y2));

    return kOfxStatOK;
}

namespace
{
    std::vector<OfxPlugin> plugins =
    {
        { kOfxImageEffectPluginApi, 1, "Toucan:ColorMap", 1, 0, ColorMapPlugin::setHostFunc, ColorMapPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Invert", 1, 0, InvertPlugin::setHostFunc, InvertPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Pow", 1, 0, PowPlugin::setHostFunc, PowPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Saturate", 1, 0, SaturatePlugin::setHostFunc, SaturatePlugin::mainEntryPoint }
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
