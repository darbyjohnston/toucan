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
    Plugin::_describeInContextAction(descriptor, inArgs);

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

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    OfxPropertySetHandle props;
    _parameterSuite->paramDefine(paramSet, kOfxParamTypeInteger2D, "size", &props);
    _propertySuite->propSetInt(props, kOfxParamPropDefault, 0, 1280);
    _propertySuite->propSetInt(props, kOfxParamPropDefault, 1, 720);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The size in pixels");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Size");

    return kOfxStatOK;
}

OfxStatus GeneratorPlugin::_createInstance(OfxImageEffectHandle descriptor)
{
    Plugin::_createInstance(descriptor);

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    _parameterSuite->paramGetHandle(paramSet, "size", &_sizeParam, nullptr);

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

OfxStatus CheckersPlugin::_describeInContextAction(
    OfxImageEffectHandle descriptor,
    OfxPropertySetHandle inArgs)
{
    GeneratorPlugin::_describeInContextAction(descriptor, inArgs);

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    OfxPropertySetHandle props;
    _parameterSuite->paramDefine(paramSet, kOfxParamTypeInteger2D, "checkerSize", &props);
    _propertySuite->propSetInt(props, kOfxParamPropDefault, 0, 100);
    _propertySuite->propSetInt(props, kOfxParamPropDefault, 1, 100);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The checker size");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Checker Size");

    _parameterSuite->paramDefine(paramSet, kOfxParamTypeRGBA, "color1", &props);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 0, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 1, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 2, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 3, 1.0);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The first color");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Color 1");

    _parameterSuite->paramDefine(paramSet, kOfxParamTypeRGBA, "color2", &props);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 0, 1.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 1, 1.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 2, 1.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 3, 1.0);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The second color");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Color 2");

    return kOfxStatOK;
}

OfxStatus CheckersPlugin::_createInstance(OfxImageEffectHandle descriptor)
{
    GeneratorPlugin::_createInstance(descriptor);

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    _parameterSuite->paramGetHandle(paramSet, "checkerSize", &_checkerSizeParam, nullptr);
    _parameterSuite->paramGetHandle(paramSet, "color1", &_color1Param, nullptr);
    _parameterSuite->paramGetHandle(paramSet, "color2", &_color2Param, nullptr);

    return kOfxStatOK;
}

OfxStatus CheckersPlugin::_render(
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    int64_t checkerSize[2] = { 0, 0 };
    double color1[4] = { 0.0, 0.0, 0.0, 0.0 };
    double color2[4] = { 0.0, 0.0, 0.0, 0.0 };
    _parameterSuite->paramGetValue(_checkerSizeParam, &checkerSize[0], &checkerSize[1]);
    _parameterSuite->paramGetValue(_color1Param, &color1[0], &color1[1], &color1[2], &color1[3]);
    _parameterSuite->paramGetValue(_color2Param, &color2[0], &color2[1], &color2[2], &color2[3]);

    OIIO::ImageBufAlgo::checker(
        outputBuf,
        checkerSize[0],
        checkerSize[1],
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

OfxStatus FillPlugin::_describeInContextAction(
    OfxImageEffectHandle descriptor,
    OfxPropertySetHandle inArgs)
{
    GeneratorPlugin::_describeInContextAction(descriptor, inArgs);

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);

    OfxPropertySetHandle props;
    _parameterSuite->paramDefine(paramSet, kOfxParamTypeRGBA, "color", &props);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 0, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 1, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 2, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 3, 0.0);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The fill color");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Color");

    return kOfxStatOK;
}

OfxStatus FillPlugin::_createInstance(OfxImageEffectHandle descriptor)
{
    GeneratorPlugin::_createInstance(descriptor);

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    _parameterSuite->paramGetHandle(paramSet, "color", &_colorParam, nullptr);

    return kOfxStatOK;
}

OfxStatus FillPlugin::_render(
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    double color[4] = { 0.0, 0.0, 0.0, 0.0 };
    _parameterSuite->paramGetValue(_colorParam, &color[0], &color[1], &color[2], &color[3]);

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

OfxStatus GradientPlugin::_describeInContextAction(
    OfxImageEffectHandle descriptor,
    OfxPropertySetHandle inArgs)
{
    GeneratorPlugin::_describeInContextAction(descriptor, inArgs);

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    OfxPropertySetHandle props;
    _parameterSuite->paramDefine(paramSet, kOfxParamTypeRGBA, "color1", &props);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 0, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 1, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 2, 0.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 3, 1.0);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The first color");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Color 1");

    _parameterSuite->paramDefine(paramSet, kOfxParamTypeRGBA, "color2", &props);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 0, 1.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 1, 1.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 2, 1.0);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 3, 1.0);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The second color");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Color 2");

    _parameterSuite->paramDefine(paramSet, kOfxParamTypeBoolean, "vertical", &props);
    _propertySuite->propSetInt(props, kOfxParamPropDefault, 0, 0);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "Whether the gradient is vertical");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Vertical");

    return kOfxStatOK;
}

OfxStatus GradientPlugin::_createInstance(OfxImageEffectHandle descriptor)
{
    GeneratorPlugin::_createInstance(descriptor);

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    _parameterSuite->paramGetHandle(paramSet, "color1", &_color1Param, nullptr);
    _parameterSuite->paramGetHandle(paramSet, "color2", &_color2Param, nullptr);
    _parameterSuite->paramGetHandle(paramSet, "vertical", &_verticalParam, nullptr);

    return kOfxStatOK;
}

OfxStatus GradientPlugin::_render(
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    double color1[4] = { 0.0, 0.0, 0.0, 0.0 };
    double color2[4] = { 0.0, 0.0, 0.0, 0.0 };
    bool vertical = false;
    _parameterSuite->paramGetValue(_color1Param, &color1[0], &color1[1], &color1[2], &color1[3]);
    _parameterSuite->paramGetValue(_color2Param, &color2[0], &color2[1], &color2[2], &color2[3]);
    _parameterSuite->paramGetValue(_verticalParam, &vertical);

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

OfxStatus NoisePlugin::_describeInContextAction(
    OfxImageEffectHandle descriptor,
    OfxPropertySetHandle inArgs)
{
    GeneratorPlugin::_describeInContextAction(descriptor, inArgs);

    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    OfxPropertySetHandle props;
    _parameterSuite->paramDefine(paramSet, kOfxParamTypeString, "type", &props);
    _propertySuite->propSetString(props, kOfxParamPropDefault, 0, "gaussian");
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The noise type");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Type");

    _parameterSuite->paramDefine(paramSet, kOfxParamTypeDouble, "a", &props);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 0, 0.0);
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "A");

    _parameterSuite->paramDefine(paramSet, kOfxParamTypeDouble, "b", &props);
    _propertySuite->propSetDouble(props, kOfxParamPropDefault, 0, 0.0);
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "B");

    _parameterSuite->paramDefine(paramSet, kOfxParamTypeBoolean, "mono", &props);
    _propertySuite->propSetInt(props, kOfxParamPropDefault, 0, 0);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "Whether the noise is monochrome");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Monochrome");

    _parameterSuite->paramDefine(paramSet, kOfxParamTypeInteger, "seed", &props);
    _propertySuite->propSetInt(props, kOfxParamPropDefault, 0, 0);
    _propertySuite->propSetString(props, kOfxParamPropHint, 0, "The random number generator seed");
    _propertySuite->propSetString(props, kOfxPropLabel, 0, "Seed");

    return kOfxStatOK;
}

OfxStatus NoisePlugin::_createInstance(OfxImageEffectHandle descriptor)
{
    GeneratorPlugin::_createInstance(descriptor);
    OfxParamSetHandle paramSet;
    _imageEffectSuite->getParamSet(descriptor, &paramSet);
    _parameterSuite->paramGetHandle(paramSet, "type", &_typeParam, nullptr);
    _parameterSuite->paramGetHandle(paramSet, "a", &_aParam, nullptr);
    _parameterSuite->paramGetHandle(paramSet, "b", &_bParam, nullptr);
    _parameterSuite->paramGetHandle(paramSet, "mono", &_monoParam, nullptr);
    _parameterSuite->paramGetHandle(paramSet, "seed", &_seedParam, nullptr);
    return kOfxStatOK;
}

OfxStatus NoisePlugin::_render(
    OIIO::ImageBuf& outputBuf,
    const OfxRectI& renderWindow,
    OfxPropertySetHandle inArgs)
{
    std::string type;
    double a = 0.0;
    double b = 0.0;
    int mono = 0;
    int seed = 0;
    _parameterSuite->paramGetValue(_typeParam, &type);
    _parameterSuite->paramGetValue(_aParam, &a);
    _parameterSuite->paramGetValue(_bParam, &b);
    _parameterSuite->paramGetValue(_monoParam, &mono);
    _parameterSuite->paramGetValue(_seedParam, &seed);

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
