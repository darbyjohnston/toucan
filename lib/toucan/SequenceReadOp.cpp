// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "SequenceReadOp.h"

#include <iomanip>
#include <sstream>

namespace toucan
{
    SequenceReadOp::SequenceReadOp(
        const std::filesystem::path& base,
        const std::string& namePrefix,
        const std::string& nameSuffix,
        int startFrame,
        int frameStep,
        double rate,
        int frameZeroPadding,
        const OTIO_NS::RationalTime& timeOffset,
        const std::vector<std::shared_ptr<IImageOp> >& inputs) :
        IImageOp(timeOffset, inputs),
        _base(base),
        _namePrefix(namePrefix),
        _nameSuffix(nameSuffix),
        _startFrame(startFrame),
        _frameStep(frameStep),
        _rate(rate),
        _frameZeroPadding(frameZeroPadding)
    {}

    SequenceReadOp::~SequenceReadOp()
    {}

    OIIO::ImageBuf SequenceReadOp::exec(const OTIO_NS::RationalTime& time)
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
        return OIIO::ImageBuf(ss.str());
    }
}
