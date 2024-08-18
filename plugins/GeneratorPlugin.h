// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Plugin.h"

#include <OpenImageIO/imagebuf.h>

class GeneratorPlugin : public Plugin
{
public:
    GeneratorPlugin(const std::string& group, const std::string& name);

    virtual ~GeneratorPlugin() = 0;

protected:
    virtual OfxStatus _render(
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

class CheckersPlugin : public GeneratorPlugin
{
public:
    CheckersPlugin();

    virtual ~CheckersPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static CheckersPlugin* _instance;
};

class FillPlugin : public GeneratorPlugin
{
public:
    FillPlugin();

    virtual ~FillPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static FillPlugin* _instance;
};

class NoisePlugin : public GeneratorPlugin
{
public:
    NoisePlugin();

    virtual ~NoisePlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static NoisePlugin* _instance;
};
