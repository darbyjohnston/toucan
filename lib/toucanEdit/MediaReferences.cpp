// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "MediaReferences.h"

#include <toucan/Util.h>

namespace toucan
{
    IMediaReference::IMediaReference()
    {}

    IMediaReference::~IMediaReference()
    {}

    ExternalReference::ExternalReference(const std::string& url) :
        _url(url)
    {}

    ExternalReference::~ExternalReference()
    {}

    const std::string& ExternalReference::getURL() const
    {
        return _url;
    }

    void ExternalReference::setURL(const std::string& url)
    {
        _url = url;
    }

    ImageSequenceReference::ImageSequenceReference(
        const std::string& targetURLBase,
        const std::string& namePrefix,
        const std::string& nameSuffix,
        int                startFrame,
        int                frameStep,
        double             rate,
        int                frameZeroPadding) :
        _targetURLBase(targetURLBase),
        _namePrefix(namePrefix),
        _nameSuffix(nameSuffix),
        _startFrame(startFrame),
        _frameStep(frameStep),
        _rate(rate),
        _frameZeroPadding(frameZeroPadding)
    {}

    ImageSequenceReference::~ImageSequenceReference()
    {}

    const std::string& ImageSequenceReference::getTargetURLBase() const
    {
        return _targetURLBase;
    }

    void ImageSequenceReference::setTargetURLBase(const std::string& value)
    {
        _targetURLBase = value;
    }

    const std::string& ImageSequenceReference::getNamePrefix() const
    {
        return _namePrefix;
    }

    void ImageSequenceReference::setNamePrefix(const std::string& value)
    {
        _namePrefix = value;
    }

    const std::string& ImageSequenceReference::getNameSuffix() const
    {
        return _nameSuffix;
    }

    void ImageSequenceReference::setNameSuffix(const std::string& value)
    {
        _nameSuffix = value;
    }

    int ImageSequenceReference::getStartFrame() const
    {
        return _startFrame;
    }

    void ImageSequenceReference::setStartFrame(int value)
    {
        _startFrame = value;
    }

    int ImageSequenceReference::getFrameStep() const
    {
        return _frameStep;
    }

    void ImageSequenceReference::setFrameStep(int value)
    {
        _frameStep = value;
    }

    double ImageSequenceReference::getRate() const
    {
        return _rate;
    }

    void ImageSequenceReference::setRate(double value)
    {
        _rate = value;
    }

    int ImageSequenceReference::getFrameZeroPadding() const
    {
        return _frameZeroPadding;
    }

    void ImageSequenceReference::setFrameZeroPadding(int value)
    {
        _frameZeroPadding = value;
    }

    GeneratorReference::GeneratorReference(
        const std::string& generatorKind,
        const OTIO_NS::AnyDictionary& parameters) :
        _generatorKind(generatorKind),
        _parameters(parameters)
    {}

    GeneratorReference::~GeneratorReference()
    {}

    const std::string& GeneratorReference::getGeneratorKind() const
    {
        return _generatorKind;
    }

    void GeneratorReference::setGeneratorKind(const std::string& value)
    {
        _generatorKind = value;
    }

    const OTIO_NS::AnyDictionary& GeneratorReference::getParameters() const
    {
        return _parameters;
    }

    void GeneratorReference::setParameters(const OTIO_NS::AnyDictionary& value)
    {
        _parameters = value;
    }

    std::filesystem::path getMediaPath(
        const std::filesystem::path& timelinePath,
        const std::string& url)
    {
        std::filesystem::path path = splitURLProtocol(url).second;
        if (!path.is_absolute())
        {
            path = timelinePath / path;
        }
        return path;
    }
}
