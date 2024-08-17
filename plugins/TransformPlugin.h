// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Plugin.h"

#include <OpenImageIO/imagebuf.h>

class TransformPlugin : public Plugin
{
public:
    TransformPlugin(const std::string& group, const std::string& name);

    virtual ~TransformPlugin() = 0;

protected:
    virtual OfxStatus _render(
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) = 0;

    OfxStatus _describeInContextAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle) override;
    OfxStatus _renderAction(
        OfxImageEffectHandle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs) override;
};

class FlipPlugin : public TransformPlugin
{
public:
    FlipPlugin();

    virtual ~FlipPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static FlipPlugin* _instance;
};

class FlopPlugin : public TransformPlugin
{
public:
    FlopPlugin();

    virtual ~FlopPlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static FlopPlugin* _instance;
};

class ResizePlugin : public TransformPlugin
{
public:
    ResizePlugin();

    virtual ~ResizePlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static ResizePlugin* _instance;
};

class RotatePlugin : public TransformPlugin
{
public:
    RotatePlugin();

    virtual ~RotatePlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        const OfxRectI& renderWindow,
        OfxPropertySetHandle inArgs) override;

private:
    static RotatePlugin* _instance;
};
