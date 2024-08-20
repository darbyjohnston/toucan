// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TransformPlugin.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

#include <Imath/ImathVec.h>

TransformPlugin::TransformPlugin(const std::string& group, const std::string& name) :
    Plugin(group, name)
{}

TransformPlugin::~TransformPlugin()
{}

OfxStatus TransformPlugin::_describeAction(OfxImageEffectHandle descriptor)
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

OfxStatus TransformPlugin::_describeInContextAction(OfxImageEffectHandle descriptor, OfxPropertySetHandle inArgs)
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

OfxStatus TransformPlugin::_renderAction(
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

CropPlugin* CropPlugin::_instance = nullptr;

CropPlugin::CropPlugin() :
    TransformPlugin("Toucan", "Crop")
{}

CropPlugin::~CropPlugin()
{}

void CropPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new CropPlugin;
    }
    _instance->_host = host;
}

OfxStatus CropPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus CropPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    IMATH_NAMESPACE::V2i pos;
    IMATH_NAMESPACE::V2i size;
    _propertySuite->propGetIntN(inArgs, "pos", 2, &pos.x);
    _propertySuite->propGetIntN(inArgs, "size", 2, &size.x);

    const auto crop = OIIO::ImageBufAlgo::cut(
        sourceBuf,
        OIIO::ROI(pos.x, pos.x + size.x, pos.y, pos.y + size.y));
    OIIO::ImageBufAlgo::copy(outputBuf, crop);

    return kOfxStatOK;
}

FlipPlugin* FlipPlugin::_instance = nullptr;

FlipPlugin::FlipPlugin() :
    TransformPlugin("Toucan", "Flip")
{}

FlipPlugin::~FlipPlugin()
{}

void FlipPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new FlipPlugin;
    }
    _instance->_host = host;
}

OfxStatus FlipPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus FlipPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    OIIO::ImageBufAlgo::flip(
        outputBuf,
        sourceBuf,
        OIIO::ROI(
            renderWindow.x1,
            renderWindow.x2,
            renderWindow.y1,
            renderWindow.y2));
    return kOfxStatOK;
}

FlopPlugin* FlopPlugin::_instance = nullptr;

FlopPlugin::FlopPlugin() :
    TransformPlugin("Toucan", "Flop")
{}

FlopPlugin::~FlopPlugin()
{}

void FlopPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new FlopPlugin;
    }
    _instance->_host = host;
}

OfxStatus FlopPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus FlopPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    OIIO::ImageBufAlgo::flop(
        outputBuf,
        sourceBuf,
        OIIO::ROI(
            renderWindow.x1,
            renderWindow.x2,
            renderWindow.y1,
            renderWindow.y2));
    return kOfxStatOK;
}

ResizePlugin* ResizePlugin::_instance = nullptr;

ResizePlugin::ResizePlugin() :
    TransformPlugin("Toucan", "Resize")
{}

ResizePlugin::~ResizePlugin()
{}

void ResizePlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new ResizePlugin;
    }
    _instance->_host = host;
}

OfxStatus ResizePlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus ResizePlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    IMATH_NAMESPACE::V2i size;
    _propertySuite->propGetIntN(inArgs, "size", 2, &size.x);

    std::string filterName = "";
    char* s = nullptr;
    _propertySuite->propGetString(inArgs, "filterName", 0, &s);
    if (s)
    {
        filterName = s;
    }

    double filterWidth = 0.0;
    _propertySuite->propGetDouble(inArgs, "filterWidth", 0, &filterWidth);

    OIIO::ImageBufAlgo::resize(
        outputBuf,
        sourceBuf,
        filterName,
        filterWidth,
        OIIO::ROI(0, size.x, 0, size.y));

    return kOfxStatOK;
}

RotatePlugin* RotatePlugin::_instance = nullptr;

RotatePlugin::RotatePlugin() :
    TransformPlugin("Toucan", "Rotate")
{}

RotatePlugin::~RotatePlugin()
{}

void RotatePlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new RotatePlugin;
    }
    _instance->_host = host;
}

OfxStatus RotatePlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus RotatePlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    double angle = 0.0;
    _propertySuite->propGetDouble(inArgs, "angle", 0, &angle);

    std::string filterName = "";
    char* s = nullptr;
    _propertySuite->propGetString(inArgs, "filterName", 0, &s);
    if (s)
    {
        filterName = s;
    }

    double filterWidth = 0.0;
    _propertySuite->propGetDouble(inArgs, "filterWidth", 0, &filterWidth);

    OIIO::ImageBufAlgo::rotate(
        outputBuf,
        sourceBuf,
        angle / 360.F * 2.F * M_PI,
        filterName,
        filterWidth);

    return kOfxStatOK;
}

namespace
{
    std::vector<OfxPlugin> plugins =
    {
        { kOfxImageEffectPluginApi, 1, "Toucan:Crop", 1, 0, CropPlugin::setHostFunc, CropPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Flip", 1, 0, FlipPlugin::setHostFunc, FlipPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Flop", 1, 0, FlopPlugin::setHostFunc, FlopPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Resize", 1, 0, ResizePlugin::setHostFunc, ResizePlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Rotate", 1, 0, RotatePlugin::setHostFunc, RotatePlugin::mainEntryPoint }
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
