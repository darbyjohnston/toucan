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
        _path(path),
        _input(OIIO::ImageInput::open(_path.string())),
        _spec(_input->spec())
    {
        if (auto param = _spec.find_attribute("oiio:subimages"))
        {
            _frameCount = param->get_int();
        }
        if (auto param = _spec.find_attribute("FramesPerSecond"))
        {
            _spec.getattribute("FramesPerSecond", OIIO::TypeDesc::TypeRational, &_fps);
        }
        if (auto param = _spec.find_attribute("ffmpeg:TimeCode"))
        {
            const std::string timecode = param->get_string();
            double rate = 24.0;
            if (_fps[0] > 0 && _fps[1] > 0)
            {
                rate = _fps[0] / static_cast<double>(_fps[1]);
            }
            _startTime = OTIO_NS::RationalTime::from_timecode(timecode, rate);
        }
    }

    ReadNode::~ReadNode()
    {
        _input->close();
    }

    std::string ReadNode::getLabel() const
    {
        std::stringstream ss;
        ss << _name << ": " << _path.filename().string();
        return ss.str();
    }

    OIIO::ImageBuf ReadNode::exec(const OTIO_NS::RationalTime& time)
    {
        OIIO::ImageBuf out;

        OTIO_NS::RationalTime offsetTime = time;
        if (!_timeOffset.is_invalid_time())
        {
            offsetTime -= _timeOffset;
        }
        if (!_startTime.is_invalid_time())
        {
            offsetTime -= _startTime;
        }
        const int subImage = std::max(
            static_cast<int>(0),
            std::min(static_cast<int>(offsetTime.floor().value()), _frameCount - 1));
        _input->seek_subimage(subImage, 0);

        auto pixels = std::unique_ptr<unsigned char[]>(
            new unsigned char[_spec.width * _spec.height * _spec.nchannels * _spec.channel_bytes()]);
        _input->read_image(
            subImage,
            0,
            0,
            _spec.nchannels,
            _spec.format,
            &pixels[0]);

        OIIO::ImageBuf buf(
            OIIO::ImageSpec(_spec.width, _spec.height, _spec.nchannels, _spec.format),
            pixels.get(),
            _spec.nchannels * _spec.channel_bytes(),
            _spec.width * _spec.nchannels * _spec.channel_bytes(),
            0);
        if (3 == _spec.nchannels)
        {
            // Add an alpha channel.
            const int channelOrder[] = { 0, 1, 2, -1 };
            const float channelValues[] = { 0, 0, 0, 1.0 };
            const std::string channelNames[] = { "", "", "", "A" };
            out = OIIO::ImageBufAlgo::channels(buf, 4, channelOrder, channelValues, channelNames);
        }
        else
        {
            OIIO::ImageBufAlgo::copy(out, buf);
            }

        return out;
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

    std::string SequenceReadNode::getLabel() const
    {
        std::stringstream ss;
        ss << "Read: " << getSequenceFrame(
            std::filesystem::path(),
            _namePrefix,
            _startFrame,
            _frameZeroPadding,
            _nameSuffix).string();
        return ss.str();
    }

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
}
