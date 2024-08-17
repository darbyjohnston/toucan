// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "DrawPlugin.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

#include <Imath/ImathVec.h>

DrawPlugin::DrawPlugin(const std::string& group, const std::string& name) :
    Plugin(group, name)
{}

DrawPlugin::~DrawPlugin()
{}

OfxStatus DrawPlugin::_describeInContextAction(OfxImageEffectHandle descriptor, OfxPropertySetHandle inArgs)
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

OfxStatus DrawPlugin::_renderAction(
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

BoxPlugin* BoxPlugin::_instance = nullptr;

BoxPlugin::BoxPlugin() :
    DrawPlugin("Toucan", "Box")
{}

BoxPlugin::~BoxPlugin()
{}

void BoxPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new BoxPlugin;
    }
    _instance->_host = host;
}

OfxStatus BoxPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus BoxPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    IMATH_NAMESPACE::V2i pos1(0, 0);
    _propertySuite->propGetIntN(inArgs, "pos1", 2, &pos1.x);

    IMATH_NAMESPACE::V2i pos2(0, 0);
    _propertySuite->propGetIntN(inArgs, "pos2", 2, &pos2.x);

    double color[4] = { 0.0, 0.0, 0.0, 0.0 };
    _propertySuite->propGetDoubleN(inArgs, "color", 4, color);

    int fill = 0;
    _propertySuite->propGetInt(inArgs, "fill", 0, &fill);

    OIIO::ImageBufAlgo::copy(outputBuf, sourceBuf);
    OIIO::ImageBufAlgo::render_box(
        outputBuf,
        pos1.x,
        pos1.y,
        pos2.x,
        pos2.y,
        {
            static_cast<float>(color[0]),
            static_cast<float>(color[1]),
            static_cast<float>(color[2]),
            static_cast<float>(color[3])
        },
        fill);

    return kOfxStatOK;
}

LinePlugin* LinePlugin::_instance = nullptr;

LinePlugin::LinePlugin() :
    DrawPlugin("Toucan", "Line")
{}

LinePlugin::~LinePlugin()
{}

void LinePlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new LinePlugin;
    }
    _instance->_host = host;
}

OfxStatus LinePlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus LinePlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    IMATH_NAMESPACE::V2i pos1(0, 0);
    _propertySuite->propGetIntN(inArgs, "pos1", 2, &pos1.x);

    IMATH_NAMESPACE::V2i pos2(0, 0);
    _propertySuite->propGetIntN(inArgs, "pos2", 2, &pos2.x);

    double color[4] = { 0.0, 0.0, 0.0, 0.0 };
    _propertySuite->propGetDoubleN(inArgs, "color", 4, color);

    int skipFirstPoint = 0;
    _propertySuite->propGetInt(inArgs, "skipFirstPoint", 0, &skipFirstPoint);

    OIIO::ImageBufAlgo::copy(outputBuf, sourceBuf);
    OIIO::ImageBufAlgo::render_line(
        outputBuf,
        pos1.x,
        pos1.y,
        pos2.x,
        pos2.y,
        {
            static_cast<float>(color[0]),
            static_cast<float>(color[1]),
            static_cast<float>(color[2]),
            static_cast<float>(color[3])
        },
        skipFirstPoint);

    return kOfxStatOK;
}

TextPlugin* TextPlugin::_instance = nullptr;

TextPlugin::TextPlugin() :
    DrawPlugin("Toucan", "Text")
{}

TextPlugin::~TextPlugin()
{}

void TextPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new TextPlugin;
    }
    _instance->_host = host;
}

OfxStatus TextPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus TextPlugin::_render(
    const OIIO::ImageBuf& sourceBuf,
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    IMATH_NAMESPACE::V2i pos(0, 0);
    _propertySuite->propGetIntN(inArgs, "pos", 2, &pos.x);

    std::string text;
    char* s = nullptr;
    _propertySuite->propGetString(inArgs, "text", 0, &s);
    if (s)
    {
        text = s;
    }

    int fontSize = 16;
    _propertySuite->propGetInt(inArgs, "fontSize", 0, &fontSize);

    std::string fontName;
    _propertySuite->propGetString(inArgs, "fontName", 0, &s);
    if (s)
    {
        fontName = s;
    }

    double color[4] = { 0.0, 0.0, 0.0, 0.0 };
    _propertySuite->propGetDoubleN(inArgs, "color", 4, color);

    OIIO::ImageBufAlgo::copy(outputBuf, sourceBuf);
    OIIO::ImageBufAlgo::render_text(
        outputBuf,
        pos.x,
        pos.y,
        text,
        fontSize,
        fontName,
        {
            static_cast<float>(color[0]),
            static_cast<float>(color[1]),
            static_cast<float>(color[2]),
            static_cast<float>(color[3])
        });

    return kOfxStatOK;
}

namespace
{
    std::vector<OfxPlugin> plugins =
    {
        { kOfxImageEffectPluginApi, 1, "Toucan:Box", 1, 0, BoxPlugin::setHostFunc, BoxPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Line", 1, 0, LinePlugin::setHostFunc, LinePlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Text", 1, 0, TextPlugin::setHostFunc, TextPlugin::mainEntryPoint }
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
