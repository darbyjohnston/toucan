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
    class ImageEffectHost;

    //! Base class for image nodes.
    class IImageNode : public std::enable_shared_from_this<IImageNode>
    {
    public:
        IImageNode(
            const std::string& name,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~IImageNode() = 0;

        //! Get the name.
        const std::string& getName() const;

        //! Get the label.
        virtual std::string getLabel() const;

        //! Get the inputs.
        const std::vector<std::shared_ptr<IImageNode> >& getInputs() const;

        //! Set the inputs.
        void setInputs(const std::vector<std::shared_ptr<IImageNode> >&);

        //! Get the time offset.
        const OTIO_NS::RationalTime& getTimeOffset() const;

        //! Set the time offset.
        //! 
        //! \todo How should time transforms be handled?
        void setTimeOffset(const OTIO_NS::RationalTime&);

        //! Execute the image operation for the given time.
        virtual OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) = 0;

        //! Generate a Grapviz graph for the given time.
        std::vector<std::string> graph(
            const OTIO_NS::RationalTime&,
            const std::string& name);

    protected:
        void _graph(
            OTIO_NS::RationalTime,
            const std::shared_ptr<IImageNode>&,
            std::vector<std::string>&);
        std::string _getGraphName() const;

        std::string _name;
        OTIO_NS::RationalTime _timeOffset;
        std::vector<std::shared_ptr<IImageNode> > _inputs;
    };
}
