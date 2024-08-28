// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Read.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

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

    OIIO::ImageBuf ReadNode::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf out;

        OTIO_NS::RationalTime offsetTime = time;
        if (!_timeOffset.is_invalid_time())
        {
            offsetTime -= _timeOffset;
        }

        OIIO::ImageSpec spec;
        int64_t frameCount = 0;
        if (auto in = OIIO::ImageInput::open(_path.string()))
        {
            spec = in->spec();
            if (auto param = spec.find_attribute("oiio:subimages"))
            {
                frameCount = param->get_int();
            }
            int fps[2] = { 0, 0 };
            if (auto param = spec.find_attribute("FramesPerSecond"))
            {
                spec.getattribute("FramesPerSecond", OIIO::TypeDesc::TypeRational, &fps);
            }
            if (auto param = spec.find_attribute("timecode"))
            {
                const std::string timecode = param->get_string();
                double rate = 24.0;
                if (fps[0] > 0 && fps[1] > 0)
                {
                    rate = fps[0] / static_cast<double>(fps[1]);
                }
                const OTIO_NS::RationalTime timecodeTime = OTIO_NS::RationalTime::from_timecode(timecode, rate);
                if (!time.is_invalid_time())
                {
                    offsetTime -= timecodeTime;
                }
            }
        }

        const int subImage = std::max(
            static_cast<int64_t>(0),
            std::min(static_cast<int64_t>(offsetTime.floor().value()), frameCount - 1));
        out = OIIO::ImageBuf(_path.string(), subImage);

        if (3 == spec.nchannels)
        {
            // Add an alpha channel.
            //const int channelorder[] = { 0, 1, 2, -1 };
            //const float channelvalues[] = { 0, 0, 0, 1.0 };
            //const std::string channelnames[] = { "", "", "", "A" };
            //out = OIIO::ImageBufAlgo::channels(out, 4, channelorder, channelvalues, channelnames);
            OIIO::ImageBuf tmp(OIIO::ImageSpec(spec.width, spec.height, 4, spec.format));
            OIIO::ImageBufAlgo::fill(tmp, { 0.F, 0.F, 0.F, 1.F });
            OIIO::ImageBufAlgo::copy(
                tmp,
                out,
                OIIO::TypeUnknown,
                OIIO::ROI(0, spec.width, 0, spec.height, 0, 1, 0, 3));
            out = tmp;
        }

        return out;
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

    OIIO::ImageBuf SequenceReadNode::exec(const OTIO_NS::RationalTime& time)
    {
        OTIO_NS::RationalTime offsetTime = time;
        if (!_timeOffset.is_invalid_time())
        {
            offsetTime -= _timeOffset;
        }

        const int frame = offsetTime.floor().to_frames();
        const std::filesystem::path path = getSequenceFrame(
            _base,
            _namePrefix,
            frame,
            _frameZeroPadding,
            _nameSuffix);
        OIIO::ImageBuf buf(path.string());

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
        ss << "Read: " << getSequenceFrame(
            std::filesystem::path(),
            _namePrefix,
            time.to_frames(),
            _frameZeroPadding,
            _nameSuffix).string();
        return ss.str();
    }
}
