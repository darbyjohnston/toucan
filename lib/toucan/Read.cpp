// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

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
    {
        try
        {
            _ffRead = std::make_shared<FFmpegRead>(path);
            _spec = _ffRead->getSpec();
            _timeRange = _ffRead->getTimeRange();
        }
        catch (const std::exception&)
        {}
        if (!_ffRead)
        {
            _input = OIIO::ImageInput::open(_path.string());
            if (_input)
            {
                _spec = _input->spec();
            }
        }
    }

    ReadNode::~ReadNode()
    {
        if (_input)
        {
            _input->close();
        }
    }

    const OTIO_NS::TimeRange& ReadNode::getTimeRange() const
    {
        return _timeRange;
    }

    std::string ReadNode::getLabel() const
    {
        std::stringstream ss;
        ss << _name << ": " << _path.filename().string();
        return ss.str();
    }

    OIIO::ImageBuf ReadNode::exec()
    {
        OIIO::ImageBuf out;

        if (_ffRead)
        {
            OTIO_NS::RationalTime offsetTime = _time;
            if (!_timeOffset.is_invalid_time())
            {
                offsetTime -= _timeOffset;
            }

            out = _ffRead->getImage(offsetTime);

            if (3 == _spec.nchannels)
            {
                // Add an alpha channel.
                const int channelOrder[] = { 0, 1, 2, -1 };
                const float channelValues[] = { 0, 0, 0, 1.0 };
                const std::string channelNames[] = { "", "", "", "A" };
                out = OIIO::ImageBufAlgo::channels(out, 4, channelOrder, channelValues, channelNames);
            }
        }
        else
        {
            auto pixels = std::unique_ptr<unsigned char[]>(
                new unsigned char[_spec.width * _spec.height * _spec.nchannels * _spec.channel_bytes()]);
            _input->read_image(
                0,
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

    OIIO::ImageBuf SequenceReadNode::exec()
    {
        OTIO_NS::RationalTime offsetTime = _time;
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
