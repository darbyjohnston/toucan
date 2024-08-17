// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

#include <filesystem>

namespace toucan
{
    //! Read node.
    class ReadNode : public IImageNode
    {
    public:
        ReadNode(
            const std::filesystem::path&,
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~ReadNode();
        
        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        std::filesystem::path _path;
    };

    //! Sequence read node.
    class SequenceReadNode : public IImageNode
    {
    public:
        SequenceReadNode(
            const std::filesystem::path& base,
            const std::string& namePrefix,
            const std::string& nameSuffix,
            int startFrame,
            int frameStep,
            double rate,
            int frameZerNodeadding,
            const std::vector<std::shared_ptr<IImageNode> > & = {});

        virtual ~SequenceReadNode();

        OIIO::ImageBuf exec(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<ImageEffectHost>&) override;

    private:
        std::filesystem::path _base;
        std::string _namePrefix;
        std::string _nameSuffix;
        int _startFrame = 1;
        int _frameStep = 1;
        double _rate = 1.0;
        int _frameZeroPadding = 0;
    };
}
