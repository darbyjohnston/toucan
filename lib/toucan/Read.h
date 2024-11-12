// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucan/FFmpegRead.h>
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

        const OTIO_NS::TimeRange& getTimeRange() const;
        
        std::string getLabel() const override;

        OIIO::ImageBuf exec() override;

    private:
        std::filesystem::path _path;
        std::shared_ptr<FFmpegRead> _ffRead;
        std::unique_ptr<OIIO::ImageInput> _input;
        OIIO::ImageSpec _spec;
        OTIO_NS::TimeRange _timeRange;
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
            const std::vector<std::shared_ptr<IImageNode> >& = {});

        virtual ~SequenceReadNode();

        std::string getLabel() const override;

        OIIO::ImageBuf exec() override;

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
