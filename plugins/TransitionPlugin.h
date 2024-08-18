// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Plugin.h"

#include <OpenImageIO/imagebuf.h>

class TransitionPlugin : public Plugin
{
public:
    TransitionPlugin(const std::string& group, const std::string& name);

    virtual ~TransitionPlugin() = 0;

protected:
    virtual OfxStatus _render(
        const OIIO::ImageBuf&,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
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

class DissolvePlugin : public TransitionPlugin
{
public:
    DissolvePlugin();

    virtual ~DissolvePlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        const OIIO::ImageBuf&,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        OfxPropertySetHandle inArgs) override;

private:
    static DissolvePlugin* _instance;
};

class HorizontalWipePlugin : public TransitionPlugin
{
public:
    HorizontalWipePlugin();

    virtual ~HorizontalWipePlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        const OIIO::ImageBuf&,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        OfxPropertySetHandle inArgs) override;

private:
    static HorizontalWipePlugin* _instance;
};

class VerticalWipePlugin : public TransitionPlugin
{
public:
    VerticalWipePlugin();

    virtual ~VerticalWipePlugin();

    static void setHostFunc(OfxHost*);

    static OfxStatus mainEntryPoint(
        const char* action,
        const void* handle,
        OfxPropertySetHandle inArgs,
        OfxPropertySetHandle outArgs);

protected:
    OfxStatus _render(
        const OIIO::ImageBuf&,
        const OIIO::ImageBuf&,
        OIIO::ImageBuf&,
        OfxPropertySetHandle inArgs) override;

private:
    static VerticalWipePlugin* _instance;
};

