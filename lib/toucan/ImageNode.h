// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageEffectHost.h>

#include <opentimelineio/effect.h>

#include <OpenImageIO/imagebuf.h>

#include <memory>
#include <vector>

namespace toucan
{
    //! Base class for image nodes.
    class IImageNode : public std::enable_shared_from_this<IImageNode>
    {
    public:
        IImageNode(const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~IImageNode() = 0;

        //! Set the time offset.
        //! 
        //! \todo How should time transforms be handled?
        void setTimeOffset(const OTIO_NS::RationalTime&);

        //! Execute the image operation for the given time.
        virtual OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) = 0;

    protected:
        OTIO_NS::RationalTime _timeOffset;
        std::vector<std::shared_ptr<IImageNode> > _inputs;
    };

    //! Base class for OTIO effects.
    class IEffect : public OTIO_NS::Effect
    {
    public:
        IEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        //! Create an image node.
        virtual std::shared_ptr<IImageNode> createNode(
            const std::vector<std::shared_ptr<IImageNode> >& inputs) = 0;

    protected:
        virtual ~IEffect() = 0;
    };
}
