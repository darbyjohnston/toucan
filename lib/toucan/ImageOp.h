// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/Host.h>

#include <opentimelineio/effect.h>

#include <OpenImageIO/imagebuf.h>

#include <memory>
#include <vector>

namespace toucan
{
    //! Base class for image operations.
    class IImageOp : public std::enable_shared_from_this<IImageOp>
    {
    public:
        IImageOp(const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~IImageOp() = 0;

        //! Set the time offset.
        //! 
        //! \todo How should time transform be handled?
        void setTimeOffset(const OTIO_NS::RationalTime&);

        //! Execute the image operation for the given time.
        virtual OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Host>&) = 0;

    protected:
        OTIO_NS::RationalTime _timeOffset;
        std::vector<std::shared_ptr<IImageOp> > _inputs;
    };

    //! Base class for OTIO effects.
    class IEffect : public OTIO_NS::Effect
    {
    public:
        IEffect(
            std::string const& name = std::string(),
            std::string const& effect_name = std::string(),
            OTIO_NS::AnyDictionary const& metadata = OTIO_NS::AnyDictionary());

        //! Create an image operation.
        virtual std::shared_ptr<IImageOp> createOp(
            const std::vector<std::shared_ptr<IImageOp> >& inputs) = 0;

    protected:
        virtual ~IEffect() = 0;
    };
}
