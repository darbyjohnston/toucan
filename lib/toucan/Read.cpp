// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Read.h"

#include <OpenImageIO/imagebufalgo.h>

#include <iomanip>
#include <sstream>

namespace toucan
{
    ReadNode::ReadNode(
        const std::filesystem::path& path,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Read", inputs),
        _path(path)
    {}

    ReadNode::~ReadNode()
    {}

    OIIO::ImageBuf ReadNode::exec(
        const OTIO_NS::RationalTime&,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OIIO::ImageBuf buf(_path.string());
        const auto& spec = buf.spec();
        if (3 == spec.nchannels)
        {
            // Add an alpha channel.
            const int channelorder[] = { 0, 1, 2, -1 };
            const float channelvalues[] = { 0, 0, 0, 1.0 };
            const std::string channelnames[] = { "", "", "", "A" };
            buf = OIIO::ImageBufAlgo::channels(buf, 4, channelorder, channelvalues, channelnames);
        }
        return buf;
    }

    std::string ReadNode::_getGraphLabel(const OTIO_NS::RationalTime&) const
    {
        std::stringstream ss;
        ss << _name << ": " << _path.filename().string();
        return ss.str();
    }

    SequenceReadNode::SequenceReadNode(
        const std::filesystem::path& base,
        const std::string& namePrefix,
        const std::string& nameSuffix,
        int startFrame,
        int frameStep,
        double rate,
        int frameZeroPadding,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("SequenceRead", inputs),
        _base(base),
        _namePrefix(namePrefix),
        _nameSuffix(nameSuffix),
        _startFrame(startFrame),
        _frameStep(frameStep),
        _rate(rate),
        _frameZeroPadding(frameZeroPadding)
    {}

    SequenceReadNode::~SequenceReadNode()
    {}

    OIIO::ImageBuf SequenceReadNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OTIO_NS::RationalTime offsetTime = time;
        if (!_timeOffset.is_invalid_time())
        {
            offsetTime -= _timeOffset;
        }
        std::stringstream ss;
        ss << _base.string() <<
            _namePrefix <<
            std::setw(_frameZeroPadding) << std::setfill('0') << offsetTime.to_frames() <<
            _nameSuffix;
        OIIO::ImageBuf buf(ss.str());
        const auto& spec = buf.spec();
        if (3 == spec.nchannels)
        {
            // Add an alpha channel.
            const int channelorder[] = { 0, 1, 2, -1 };
            const float channelvalues[] = { 0, 0, 0, 1.0 };
            const std::string channelnames[] = { "", "", "", "A" };
            buf = OIIO::ImageBufAlgo::channels(buf, 4, channelorder, channelvalues, channelnames);
        }
        return buf;
    }

    std::string SequenceReadNode::_getGraphLabel(const OTIO_NS::RationalTime& time) const
    {
        std::stringstream ss;
        ss << "Read: " <<
            _namePrefix <<
            std::setw(_frameZeroPadding) << std::setfill('0') << time.to_frames() <<
            _nameSuffix;
        return ss.str();
    }
}
