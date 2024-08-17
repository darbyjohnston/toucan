// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "InvertPlugin.h"

#include "Util.h"

#include <OpenFX/ofxImageEffect.h>

#include <OpenImageIO/imagebufalgo.h>

#include <iostream>

namespace
{
    OfxHost* host = nullptr;
    OfxPropertySuiteV1* propertySuite = nullptr;
    OfxImageEffectSuiteV1* imageEffectSuite = nullptr;
    OfxPlugin effectPluginStruct =
    {
        kOfxImageEffectPluginApi,
        1,
        "Toucan:Invert",
        1,
        0,
        SetHostFunc,
        MainEntryPoint
    };
}

extern "C"
{
    int OfxGetNumberOfPlugins(void)
    {
        return 1;
    }

    OfxPlugin* OfxGetPlugin(int index)
    {
        if (0 == index)
        {
            return &effectPluginStruct;
        }
        return 0;
    }

    void SetHostFunc(OfxHost* value)
    {
        host = value;
    }

    OfxStatus LoadAction(void)
    {
        propertySuite = (OfxPropertySuiteV1*)host->fetchSuite(
            host->host,
            kOfxPropertySuite,
            1);
        imageEffectSuite = (OfxImageEffectSuiteV1*)host->fetchSuite(
            host->host,
            kOfxImageEffectSuite,
            1);
        return kOfxStatOK;
    }

    OfxStatus UnloadAction(void)
    {
        imageEffectSuite = nullptr;
        propertySuite = nullptr;
        host = nullptr;
        return kOfxStatOK;
    }

    OfxStatus DescribeAction(OfxImageEffectHandle descriptor)
    {
        OfxPropertySetHandle effectProps;
        imageEffectSuite->getPropertySet(descriptor, &effectProps);
        propertySuite->propSetString(
            effectProps,
            kOfxPropLabel,
            0,
            "Invert");
        propertySuite->propSetString(
            effectProps,
            kOfxImageEffectPluginPropGrouping,
            0,
            "Toucan");
        propertySuite->propSetString(
            effectProps,
            kOfxImageEffectPropSupportedContexts,
            0,
            kOfxImageEffectContextFilter);
        return kOfxStatOK;
    }

    OfxStatus DescribeInContextAction(OfxImageEffectHandle descriptor, OfxPropertySetHandle inArgs)
    {
        OfxPropertySetHandle sourceProps;
        OfxPropertySetHandle outputProps;
        imageEffectSuite->clipDefine(descriptor, "Source", &sourceProps);
        imageEffectSuite->clipDefine(descriptor, "Output", &outputProps);
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
            propertySuite->propSetString(
                sourceProps,
                kOfxImageEffectPropSupportedComponents,
                i,
                components[i].c_str());
            propertySuite->propSetString(
                outputProps,
                kOfxImageEffectPropSupportedComponents,
                i,
                components[i].c_str());
        }
        for (int i = 0; i < pixelDepths.size(); ++i)
        {
            propertySuite->propSetString(
                sourceProps,
                kOfxImageEffectPropSupportedPixelDepths,
                i,
                pixelDepths[i].c_str());
            propertySuite->propSetString(
                outputProps,
                kOfxImageEffectPropSupportedPixelDepths,
                i,
                pixelDepths[i].c_str());
        }
        return kOfxStatOK;
    }

    OfxStatus RenderAction(
        OfxImageEffectHandle instance,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs)
    {
        OfxTime time;
        OfxRectI renderWindow;
        propertySuite->propGetDouble(inArgs, kOfxPropTime, 0, &time);
        propertySuite->propGetIntN(inArgs, kOfxImageEffectPropRenderWindow, 4, &renderWindow.x1);

        OfxImageClipHandle sourceClip = nullptr;
        OfxImageClipHandle outputClip = nullptr;
        OfxPropertySetHandle sourceImage = nullptr;
        OfxPropertySetHandle outputImage = nullptr;
        imageEffectSuite->clipGetHandle(instance, "Source", &sourceClip, nullptr);
        imageEffectSuite->clipGetHandle(instance, "Output", &outputClip, nullptr);
        if (sourceClip && outputClip)
        {
            imageEffectSuite->clipGetImage(sourceClip, time, nullptr, &sourceImage);
            imageEffectSuite->clipGetImage(outputClip, time, nullptr, &outputImage);
            if (sourceImage && outputImage)
            {
                const OIIO::ImageBuf sourceBuf = propSetToBuf(propertySuite, sourceImage);
                OIIO::ImageBuf outputBuf = propSetToBuf(propertySuite, outputImage);
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
            }
        }

        if (sourceImage)
        {
            imageEffectSuite->clipReleaseImage(sourceImage);
        }
        if (outputImage)
        {
            imageEffectSuite->clipReleaseImage(outputImage);
        }
        return kOfxStatOK;
    }

    OfxStatus MainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs)
    {
        OfxStatus out = kOfxStatReplyDefault;
        //std::cout << "MainEntryPoint: " << action << std::endl;
        OfxImageEffectHandle effectHandle = (OfxImageEffectHandle)handle;
        if (strcmp(action, kOfxActionLoad) == 0)
        {
            out = LoadAction();
        }
        else if (strcmp(action, kOfxActionUnload) == 0)
        {
            out = UnloadAction();
        }
        else if (strcmp(action, kOfxActionDescribe) == 0)
        {
            out = DescribeAction(effectHandle);
        }
        else if (strcmp(action, kOfxImageEffectActionDescribeInContext) == 0)
        {
            out = DescribeInContextAction(effectHandle, inArgs);
        }
        else if (strcmp(action, kOfxImageEffectActionRender) == 0)
        {
            out = RenderAction(effectHandle, inArgs, outArgs);
        }
        return out;
    }
}