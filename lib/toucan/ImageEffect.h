// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>
#include <toucan/Plugin.h>
#include <toucan/PropertySet.h>

#include <OpenFX/ofxImageEffect.h>

#include <opentimelineio/anyDictionary.h>

namespace toucan
{
    //! Image effect plugin.
    struct ImageEffectPlugin;

    //! Image effect instance.
    struct ImageEffectInstance;

    //! Image effect handle.
    struct ImageEffectHandle
    {
        ImageEffectPlugin* plugin = nullptr;
        ImageEffectInstance* instance = nullptr;
    };

    //! Image effect node.
    class ImageEffectNode : public IImageNode
    {
    public:
        ImageEffectNode(
            ImageEffectPlugin&,
            const std::string& name,
            const OTIO_NS::AnyDictionary& metaData,
            const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~ImageEffectNode();

        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

    private:
        ImageEffectPlugin& _plugin;
        std::unique_ptr<ImageEffectInstance> _instance;
        ImageEffectHandle _handle;
        OTIO_NS::AnyDictionary _metaData;
    };
}
