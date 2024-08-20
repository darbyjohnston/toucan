// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TransitionPlugin.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

#include <Imath/ImathVec.h>

TransitionPlugin::TransitionPlugin(const std::string& group, const std::string& name) :
    Plugin(group, name)
{}

TransitionPlugin::~TransitionPlugin()
{}

OfxStatus TransitionPlugin::_describeAction(OfxImageEffectHandle descriptor)
{
    Plugin::_describeAction(descriptor);
    OfxPropertySetHandle effectProps;
    _imageEffectSuite->getPropertySet(descriptor, &effectProps);
    _propertySuite->propSetString(
        effectProps,
        kOfxImageEffectPropSupportedContexts,
        0,
        kOfxImageEffectContextTransition);
    return kOfxStatOK;
}

OfxStatus TransitionPlugin::_describeInContextAction(OfxImageEffectHandle descriptor, OfxPropertySetHandle inArgs)
{
    OfxPropertySetHandle sourceFromProps;
    OfxPropertySetHandle sourceToProps;
    OfxPropertySetHandle outputProps;
    _imageEffectSuite->clipDefine(descriptor, "SourceFrom", &sourceFromProps);
    _imageEffectSuite->clipDefine(descriptor, "SourceTo", &sourceToProps);
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
            sourceFromProps,
            kOfxImageEffectPropSupportedComponents,
            i,
            components[i].c_str());
        _propertySuite->propSetString(
            sourceToProps,
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
            sourceFromProps,
            kOfxImageEffectPropSupportedPixelDepths,
            i,
            pixelDepths[i].c_str());
        _propertySuite->propSetString(
            sourceToProps,
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

OfxStatus TransitionPlugin::_renderAction(
    OfxImageEffectHandle instance,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    OfxTime time;
    OfxRectI renderWindow;
    _propertySuite->propGetDouble(inArgs, kOfxPropTime, 0, &time);
    _propertySuite->propGetIntN(inArgs, kOfxImageEffectPropRenderWindow, 4, &renderWindow.x1);

    OfxImageClipHandle sourceFromClip = nullptr;
    OfxImageClipHandle sourceToClip = nullptr;
    OfxImageClipHandle outputClip = nullptr;
    OfxPropertySetHandle sourceFromImage = nullptr;
    OfxPropertySetHandle sourceToImage = nullptr;
    OfxPropertySetHandle outputImage = nullptr;
    _imageEffectSuite->clipGetHandle(instance, "SourceFrom", &sourceFromClip, nullptr);
    _imageEffectSuite->clipGetHandle(instance, "SourceTo", &sourceToClip, nullptr);
    _imageEffectSuite->clipGetHandle(instance, "Output", &outputClip, nullptr);
    if (sourceFromClip && sourceToClip && outputClip)
    {
        _imageEffectSuite->clipGetImage(sourceFromClip, time, nullptr, &sourceFromImage);
        _imageEffectSuite->clipGetImage(sourceToClip, time, nullptr, &sourceToImage);
        _imageEffectSuite->clipGetImage(outputClip, time, nullptr, &outputImage);
        if (sourceFromImage && sourceToImage && outputImage)
        {
            const OIIO::ImageBuf sourceFromBuf = propSetToBuf(_propertySuite, sourceFromImage);
            const OIIO::ImageBuf sourceToBuf = propSetToBuf(_propertySuite, sourceToImage);
            OIIO::ImageBuf outputBuf = propSetToBuf(_propertySuite, outputImage);
            _render(sourceFromBuf, sourceToBuf, outputBuf,  inArgs);
        }
    }

    if (sourceFromImage)
    {
        _imageEffectSuite->clipReleaseImage(sourceFromImage);
    }
    if (sourceToImage)
    {
        _imageEffectSuite->clipReleaseImage(sourceToImage);
    }
    if (outputImage)
    {
        _imageEffectSuite->clipReleaseImage(outputImage);
    }
    return kOfxStatOK;
}

DissolvePlugin* DissolvePlugin::_instance = nullptr;

DissolvePlugin::DissolvePlugin() :
    TransitionPlugin("Toucan", "Dissolve")
{}

DissolvePlugin::~DissolvePlugin()
{}

void DissolvePlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new DissolvePlugin;
    }
    _instance->_host = host;
}

OfxStatus DissolvePlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus DissolvePlugin::_render(
    const OIIO::ImageBuf& sourceFromBuf,
    const OIIO::ImageBuf& sourceToBuf,
    OIIO::ImageBuf& outputBuf,
    OfxPropertySetHandle inArgs)
{
    double value = 0.0;
    _propertySuite->propGetDouble(inArgs, "value", 0, &value);

    const float v = value;
    const float iv = 1.0 - value;
    OIIO::ImageBufAlgo::add(
        outputBuf,
        OIIO::ImageBufAlgo::mul(
            sourceFromBuf,
            OIIO::ImageBufAlgo::fill({ iv, iv, iv, iv }, sourceFromBuf.roi())),
        OIIO::ImageBufAlgo::mul(
            sourceToBuf,
            OIIO::ImageBufAlgo::fill({ v, v, v, v }, sourceToBuf.roi())));

    return kOfxStatOK;
}

HorizontalWipePlugin* HorizontalWipePlugin::_instance = nullptr;

HorizontalWipePlugin::HorizontalWipePlugin() :
    TransitionPlugin("Toucan", "HorizontalWipe")
{}

HorizontalWipePlugin::~HorizontalWipePlugin()
{}

void HorizontalWipePlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new HorizontalWipePlugin;
    }
    _instance->_host = host;
}

OfxStatus HorizontalWipePlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus HorizontalWipePlugin::_render(
    const OIIO::ImageBuf& sourceFromBuf,
    const OIIO::ImageBuf& sourceToBuf,
    OIIO::ImageBuf& outputBuf,
    OfxPropertySetHandle inArgs)
{
    double value = 0.0;
    _propertySuite->propGetDouble(inArgs, "value", 0, &value);

    const int w = sourceFromBuf.spec().width;
    const int h = sourceFromBuf.spec().height;
    const int x = w * value;
    const int wipeSize = 200;

    // Create the source from matte.
    auto sourceFromMatte = OIIO::ImageBuf(OIIO::ImageSpec(w, h, 4));
    OIIO::ImageBufAlgo::fill(sourceFromMatte, { 0.F, 0.F, 0.F, 0.F });
    OIIO::ImageBufAlgo::fill(
        sourceFromMatte,
        { 1.F, 1.F, 1.F, 1.F },
        OIIO::ROI(x, w, 0, h));
    auto gradient = OIIO::ImageBuf(OIIO::ImageSpec(h, wipeSize, 4));
    OIIO::ImageBufAlgo::fill(
        gradient,
        { 0.F, 0.F, 0.F, 0.F },
        { 1.F, 1.F, 1.F, 1.F },
        OIIO::ROI());
    OIIO::ImageBufAlgo::paste(
        sourceFromMatte,
        x, 0, 0, 0,
        OIIO::ImageBufAlgo::rotate270(gradient));

    // Create the source to matte.
    auto sourceToMatte = OIIO::ImageBuf(OIIO::ImageSpec(w, h, 4));
    OIIO::ImageBufAlgo::fill(sourceToMatte, { 0.F, 0.F, 0.F, 0.F });
    OIIO::ImageBufAlgo::fill(
        sourceToMatte,
        { 1.F, 1.F, 1.F, 1.F },
        OIIO::ROI(0, x, 0, h));
    OIIO::ImageBufAlgo::paste(
        sourceToMatte,
        x, 0, 0, 0,
        OIIO::ImageBufAlgo::rotate90(gradient));

    // Multiply the sources by the mattes and add the results together.
    OIIO::ImageBufAlgo::add(
        outputBuf,
        OIIO::ImageBufAlgo::mul(sourceFromBuf, sourceFromMatte),
        OIIO::ImageBufAlgo::mul(sourceToBuf, sourceToMatte));

    /*OIIO::ImageBufAlgo::copy(
        outputBuf,
        sourceFromBuf,
        OIIO::TypeUnknown,
        OIIO::ROI(x, w, 0, h));
    OIIO::ImageBufAlgo::copy(
        outputBuf,
        sourceToBuf,
        OIIO::TypeUnknown,
        OIIO::ROI(0, x, 0, h));*/

    return kOfxStatOK;
}

VerticalWipePlugin* VerticalWipePlugin::_instance = nullptr;

VerticalWipePlugin::VerticalWipePlugin() :
    TransitionPlugin("Toucan", "VerticalWipe")
{}

VerticalWipePlugin::~VerticalWipePlugin()
{}

void VerticalWipePlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new VerticalWipePlugin;
    }
    _instance->_host = host;
}

OfxStatus VerticalWipePlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus VerticalWipePlugin::_render(
    const OIIO::ImageBuf& sourceFromBuf,
    const OIIO::ImageBuf& sourceToBuf,
    OIIO::ImageBuf& outputBuf,
    OfxPropertySetHandle inArgs)
{
    double value = 0.0;
    _propertySuite->propGetDouble(inArgs, "value", 0, &value);

    const int w = sourceFromBuf.spec().width;
    const int h = sourceFromBuf.spec().height;
    const int y = h * value;
    const int x = w * value;
    const int wipeSize = 200;

    // Create the source from matte.
    auto sourceFromMatte = OIIO::ImageBuf(OIIO::ImageSpec(w, h, 4));
    OIIO::ImageBufAlgo::fill(sourceFromMatte, { 0.F, 0.F, 0.F, 0.F });
    OIIO::ImageBufAlgo::fill(
        sourceFromMatte,
        { 1.F, 1.F, 1.F, 1.F },
        OIIO::ROI(0, w, y, h));
    auto gradient = OIIO::ImageBuf(OIIO::ImageSpec(w, wipeSize, 4));
    OIIO::ImageBufAlgo::fill(
        gradient,
        { 0.F, 0.F, 0.F, 0.F },
        { 1.F, 1.F, 1.F, 1.F },
        OIIO::ROI());
    OIIO::ImageBufAlgo::paste(
        sourceFromMatte,
        0, y, 0, 0,
        gradient);

    // Create the source to matte.
    auto sourceToMatte = OIIO::ImageBuf(OIIO::ImageSpec(w, h, 4));
    OIIO::ImageBufAlgo::fill(sourceToMatte, { 0.F, 0.F, 0.F, 0.F });
    OIIO::ImageBufAlgo::fill(
        sourceToMatte,
        { 1.F, 1.F, 1.F, 1.F },
        OIIO::ROI(0, w, 0, y));
    OIIO::ImageBufAlgo::paste(
        sourceToMatte,
        0, y, 0, 0,
        OIIO::ImageBufAlgo::rotate180(gradient));

    // Multiply the sources by the mattes and add the results together.
    OIIO::ImageBufAlgo::add(
        outputBuf,
        OIIO::ImageBufAlgo::mul(sourceFromBuf, sourceFromMatte),
        OIIO::ImageBufAlgo::mul(sourceToBuf, sourceToMatte));

    /*OIIO::ImageBufAlgo::copy(
        outputBuf,
        sourceFromBuf,
        OIIO::TypeUnknown,
        OIIO::ROI(0, w, y, h));
    OIIO::ImageBufAlgo::copy(
        outputBuf,
        sourceToBuf,
        OIIO::TypeUnknown,
        OIIO::ROI(0, w, 0, y));*/

    return kOfxStatOK;
}

namespace
{
    std::vector<OfxPlugin> plugins =
    {
        { kOfxImageEffectPluginApi, 1, "Toucan:Dissolve", 1, 0, DissolvePlugin::setHostFunc, DissolvePlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:HorizontalWipe", 1, 0, HorizontalWipePlugin::setHostFunc, HorizontalWipePlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:VerticalWipe", 1, 0, VerticalWipePlugin::setHostFunc, VerticalWipePlugin::mainEntryPoint }
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
