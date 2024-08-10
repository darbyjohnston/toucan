// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

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
        virtual ~IImageOp() = 0;

        virtual void setInputs(const std::vector<std::shared_ptr<IImageOp> >&);

        virtual OIIO::ImageBuf exec() = 0;

    protected:
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

        virtual std::shared_ptr<IImageOp> createOp() = 0;

    protected:
        virtual ~IEffect() = 0;
    };
}
