// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <filesystem>

namespace toucan
{
    //! Sequence read operation.
    class SequenceReadOp : public IImageOp
    {
    public:
        SequenceReadOp(
            const std::filesystem::path& base,
            const std::string& namePrefix,
            const std::string& nameSuffix,
            int startFrame,
            int frameStep,
            double rate,
            int frameZeroPadding,
            const OTIO_NS::RationalTime& = OTIO_NS::RationalTime(),
            const std::vector<std::shared_ptr<IImageOp> >& = {});

        virtual ~SequenceReadOp();
        
        OIIO::ImageBuf exec(const OTIO_NS::RationalTime&) override;

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
