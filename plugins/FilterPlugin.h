// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "Plugin.h"

#include <OpenImageIO/imagebuf.h>

class FilterPlugin : public Plugin
{
public:
    FilterPlugin(const std::string& group, const std::string& name);

    virtual ~FilterPlugin() = 0;

protected:
    virtual OfxStatus _render(
        OfxImageEffectHandle,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) = 0;

    OfxStatus _describeAction(OfxImageEffectHandle) override;
    OfxStatus _describeInContextAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle) override;
    OfxStatus _renderAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs) override;
};

class BlurPlugin : public FilterPlugin
{
public:
    BlurPlugin();

    virtual ~BlurPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _describeInContextAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle) override;
    OfxStatus _render(
        OfxImageEffectHandle,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static BlurPlugin* _plugin;
};

class ColorMapPlugin : public FilterPlugin
{
public:
    ColorMapPlugin();

    virtual ~ColorMapPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _describeInContextAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle) override;
    OfxStatus _render(
        OfxImageEffectHandle,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static ColorMapPlugin* _plugin;
};

class InvertPlugin : public FilterPlugin
{
public:
    InvertPlugin();

    virtual ~InvertPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        OfxImageEffectHandle,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static InvertPlugin* _plugin;
};

class PowPlugin : public FilterPlugin
{
public:
    PowPlugin();

    virtual ~PowPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _describeInContextAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle) override;
    OfxStatus _render(
        OfxImageEffectHandle,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static PowPlugin* _plugin;
};

class SaturatePlugin : public FilterPlugin
{
public:
    SaturatePlugin();

    virtual ~SaturatePlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _describeInContextAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle) override;
    OfxStatus _render(
        OfxImageEffectHandle,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static SaturatePlugin* _plugin;
};

class UnsharpMaskPlugin : public FilterPlugin
{
public:
    UnsharpMaskPlugin();

    virtual ~UnsharpMaskPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _describeInContextAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle) override;
    OfxStatus _render(
        OfxImageEffectHandle,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static UnsharpMaskPlugin* _plugin;
};
