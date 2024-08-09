// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <opentime/rationalTime.h>

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
    };
    
    //! Base class for image generators.
    class IImageGenerator : public IImageOp
    {
    public:
        virtual ~IImageGenerator() = 0;
        
        virtual OIIO::ImageBuf exec() = 0;
    };

    //! Base class for image filters.
    class IImageFilter : public IImageGenerator
    {
    public:
        virtual ~IImageFilter() = 0;

        virtual void setInputs(const std::vector<std::shared_ptr<IImageGenerator> >&) = 0;
    };
}
