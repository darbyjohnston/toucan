// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <opentimelineio/anyDictionary.h>

#include <memory>
#include <filesystem>
#include <string>

namespace toucan
{
    class IMediaReference : public std::enable_shared_from_this<IMediaReference>
    {
    public:
        IMediaReference();

        virtual ~IMediaReference() = 0;
    };

    class ExternalReference : public IMediaReference
    {
    public:
        ExternalReference(const std::string& url = std::string());

        ~ExternalReference();

        const std::string& getURL() const;
        void setURL(const std::string&);

    private:
        std::string _url;
    };

    class ImageSequenceReference : public IMediaReference
    {
    public:
        ImageSequenceReference(
            const std::string& targetURLBase = std::string(),
            const std::string& namePrefix = std::string(),
            const std::string& nameSuffix = std::string(),
            int                startFrame = 1,
            int                frameStep = 1,
            double             rate = 1.0,
            int                frameZeroPadding = 0);

        ~ImageSequenceReference();

        const std::string& getTargetURLBase() const;
        void setTargetURLBase(const std::string&);

        const std::string& getNamePrefix() const;
        void setNamePrefix(const std::string&);

        const std::string& getNameSuffix() const;
        void setNameSuffix(const std::string&);

        int getStartFrame() const;
        void setStartFrame(int);

        int getFrameStep() const;
        void setFrameStep(int);

        double getRate() const;
        void setRate(double);

        int getFrameZeroPadding() const;
        void setFrameZeroPadding(int);

    private:
        std::string _targetURLBase;
        std::string _namePrefix;
        std::string _nameSuffix;
        int         _startFrame       = 1;
        int         _frameStep        = 1;
        double      _rate             = 1.0;
        int         _frameZeroPadding = 0;
    };

    class GeneratorReference : public IMediaReference
    {
    public:
        GeneratorReference(
            const std::string& generatorKind,
            const OTIO_NS::AnyDictionary& parameters);

        ~GeneratorReference();

        const std::string& getGeneratorKind() const;
        void setGeneratorKind(const std::string&);

        const OTIO_NS::AnyDictionary& getParameters() const;
        void setParameters(const OTIO_NS::AnyDictionary&);

    private:
        std::string _generatorKind;
        OTIO_NS::AnyDictionary _parameters;
    };

    std::filesystem::path getMediaPath(
        const std::filesystem::path& timelinePath,
        const std::string& url);
}

