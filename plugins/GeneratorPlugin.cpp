// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "GeneratorPlugin.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

#include <Imath/ImathVec.h>

GeneratorPlugin::GeneratorPlugin(const std::string& group, const std::string& name) :
    Plugin(group, name)
{}

GeneratorPlugin::~GeneratorPlugin()
{}

OfxStatus GeneratorPlugin::_describeAction(OfxImageEffectHandle descriptor)
{
    Plugin::_describeAction(descriptor);
    OfxPropertySetHandle effectProps;
    _imageEffectSuite->getPropertySet(descriptor, &effectProps);
    _propertySuite->propSetString(
        effectProps,
        kOfxImageEffectPropSupportedContexts,
        0,
        kOfxImageEffectContextGenerator);
    return kOfxStatOK;
}

OfxStatus GeneratorPlugin::_describeInContextAction(OfxImageEffectHandle descriptor, OfxPropertySetHandle inArgs)
{
    OfxPropertySetHandle outputProps;
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
            outputProps,
            kOfxImageEffectPropSupportedComponents,
            i,
            components[i].c_str());
    }
    for (int i = 0; i < pixelDepths.size(); ++i)
    {
        _propertySuite->propSetString(
            outputProps,
            kOfxImageEffectPropSupportedPixelDepths,
            i,
            pixelDepths[i].c_str());
    }
    return kOfxStatOK;
}

OfxStatus GeneratorPlugin::_renderAction(
    OfxImageEffectHandle instance,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    OfxTime time;
    OfxRectI renderWindow;
    _propertySuite->propGetDouble(inArgs, kOfxPropTime, 0, &time);
    _propertySuite->propGetIntN(inArgs, kOfxImageEffectPropRenderWindow, 4, &renderWindow.x1);

    OfxImageClipHandle outputClip = nullptr;
    OfxPropertySetHandle outputImage = nullptr;
    _imageEffectSuite->clipGetHandle(instance, "Output", &outputClip, nullptr);
    if (outputClip)
    {
        _imageEffectSuite->clipGetImage(outputClip, time, nullptr, &outputImage);
        if (outputImage)
        {
            OIIO::ImageBuf outputBuf = propSetToBuf(_propertySuite, outputImage);
            _render(outputBuf, renderWindow, inArgs);
        }
    }

    if (outputImage)
    {
        _imageEffectSuite->clipReleaseImage(outputImage);
    }
    return kOfxStatOK;
}

CheckersPlugin* CheckersPlugin::_instance = nullptr;

CheckersPlugin::CheckersPlugin() :
    GeneratorPlugin("Toucan", "Checkers")
{}

CheckersPlugin::~CheckersPlugin()
{}

void CheckersPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new CheckersPlugin;
    }
    _instance->_host = host;
}

OfxStatus CheckersPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus CheckersPlugin::_render(
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    IMATH_NAMESPACE::V2i checkerSize(0, 0);
    _propertySuite->propGetIntN(inArgs, "checkerSize", 2, &checkerSize.x);

    double color1[4] = { 0.0, 0.0, 0.0, 0.0 };
    _propertySuite->propGetDoubleN(inArgs, "color1", 4, color1);

    double color2[4] = { 0.0, 0.0, 0.0, 0.0 };
    _propertySuite->propGetDoubleN(inArgs, "color2", 4, color2);

    OIIO::ImageBufAlgo::checker(
        outputBuf,
        checkerSize.x,
        checkerSize.y,
        1,
        {
            static_cast<float>(color1[0]),
            static_cast<float>(color1[1]),
            static_cast<float>(color1[2]),
            static_cast<float>(color1[3])
        },
        {
            static_cast<float>(color2[0]),
            static_cast<float>(color2[1]),
            static_cast<float>(color2[2]),
            static_cast<float>(color2[3])
        });

    return kOfxStatOK;
}

FillPlugin* FillPlugin::_instance = nullptr;

FillPlugin::FillPlugin() :
    GeneratorPlugin("Toucan", "Fill")
{}

FillPlugin::~FillPlugin()
{}

void FillPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new FillPlugin;
    }
    _instance->_host = host;
}

OfxStatus FillPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus FillPlugin::_render(
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    double color[4] = { 0.0, 0.0, 0.0, 0.0 };
    _propertySuite->propGetDoubleN(inArgs, "color", 4, color);

    OIIO::ImageBufAlgo::fill(
        outputBuf,
        {
            static_cast<float>(color[0]),
            static_cast<float>(color[1]),
            static_cast<float>(color[2]),
            static_cast<float>(color[3])
        });

    return kOfxStatOK;
}

GradientPlugin* GradientPlugin::_instance = nullptr;

GradientPlugin::GradientPlugin() :
    GeneratorPlugin("Toucan", "Gradient")
{}

GradientPlugin::~GradientPlugin()
{}

void GradientPlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new GradientPlugin;
    }
    _instance->_host = host;
}

OfxStatus GradientPlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus GradientPlugin::_render(
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    double color1[4] = { 0.0, 0.0, 0.0, 0.0 };
    double color2[4] = { 0.0, 0.0, 0.0, 0.0 };
    _propertySuite->propGetDoubleN(inArgs, "color1", 4, color1);
    _propertySuite->propGetDoubleN(inArgs, "color2", 4, color2);
    int vertical = 0;
    _propertySuite->propGetInt(inArgs, "vertical", 0, &vertical);

    if (vertical)
    {
        OIIO::ImageBufAlgo::fill(
            outputBuf,
            {
                static_cast<float>(color1[0]),
                static_cast<float>(color1[1]),
                static_cast<float>(color1[2]),
                static_cast<float>(color1[3])
            },
            {
                static_cast<float>(color2[0]),
                static_cast<float>(color2[1]),
                static_cast<float>(color2[2]),
                static_cast<float>(color2[3])
            },
            OIIO::ROI());
    }
    else
    {
        const auto& spec = outputBuf.spec();
        auto gradient = OIIO::ImageBuf(OIIO::ImageSpec(spec.height, spec.width, spec.nchannels));
        OIIO::ImageBufAlgo::fill(
            gradient,
            {
                static_cast<float>(color1[0]),
                static_cast<float>(color1[1]),
                static_cast<float>(color1[2]),
                static_cast<float>(color1[3])
            },
            {
                static_cast<float>(color2[0]),
                static_cast<float>(color2[1]),
                static_cast<float>(color2[2]),
                static_cast<float>(color2[3])
            },
            OIIO::ROI());
        OIIO::ImageBufAlgo::copy(
            outputBuf,
            OIIO::ImageBufAlgo::rotate270(gradient));
    }

    return kOfxStatOK;
}

NoisePlugin* NoisePlugin::_instance = nullptr;

NoisePlugin::NoisePlugin() :
    GeneratorPlugin("Toucan", "Noise")
{}

NoisePlugin::~NoisePlugin()
{}

void NoisePlugin::setHostFunc(OfxHost* host)
{
    if (!_instance)
    {
        _instance = new NoisePlugin;
    }
    _instance->_host = host;
}

OfxStatus NoisePlugin::mainEntryPoint(
    const char* action,
    const void* handle,
    OfxPropertySetHandle inArgs,
    OfxPropertySetHandle outArgs)
{
    return _instance->_entryPoint(action, handle, inArgs, outArgs);
}

OfxStatus NoisePlugin::_render(
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    std::string type;
    char* s = nullptr;
    _propertySuite->propGetString(inArgs, "type", 0, &s);
    if (s)
    {
        type = s;
    }

    double a = 0.0;
    double b = 0.0;
    _propertySuite->propGetDouble(inArgs, "a", 0, &a);
    _propertySuite->propGetDouble(inArgs, "b", 0, &b);

    int mono = 0;
    _propertySuite->propGetInt(inArgs, "mono", 0, &mono);

    int seed = 0;
    _propertySuite->propGetInt(inArgs, "seed", 0, &seed);

    OIIO::ImageBufAlgo::noise(
        outputBuf,
        type,
        a,
        b,
        mono,
        seed);

    return kOfxStatOK;
}

namespace
{
    std::vector<OfxPlugin> plugins =
    {
        { kOfxImageEffectPluginApi, 1, "Toucan:Checkers", 1, 0, CheckersPlugin::setHostFunc, CheckersPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Fill", 1, 0, FillPlugin::setHostFunc, FillPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Gradient", 1, 0, GradientPlugin::setHostFunc, GradientPlugin::mainEntryPoint },
        { kOfxImageEffectPluginApi, 1, "Toucan:Noise", 1, 0, NoisePlugin::setHostFunc, NoisePlugin::mainEntryPoint }
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
