// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "App.h"

#include <toucanUtil/File.h>

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    App::App(std::vector<std::string>& argv)
    {
        _exe = argv.front();
        argv.erase(argv.begin());

        _args.list.push_back(std::make_shared<CmdLineValueArg<std::string> >(
            _args.input,
            "input",
            "Input .otio file."));
        auto outArg = std::make_shared<CmdLineValueArg<std::string> >(
            _args.output,
            "output",
            "Output image file.");
        _args.list.push_back(outArg);

        _options.list.push_back(std::make_shared<CmdLineFlagOption>(
            _options.verbose,
            std::vector<std::string>{ "-v" },
            "Print verbose output."));
        _options.list.push_back(std::make_shared<CmdLineFlagOption>(
            _options.help,
            std::vector<std::string>{ "-h" },
            "Print help."));

        if (!argv.empty())
        {
            for (const auto& option : _options.list)
            {
                option->parse(argv);
            }
            if (!_options.help)
            {
                for (const auto& arg : _args.list)
                {
                    arg->parse(argv);
                }
                if (argv.size())
                {
                    _options.help = true;
                }
            }
        }
        else
        {
            _options.help = true;
        }
    }
        
    App::~App()
    {}
    
    int App::run()
    {
        if (_options.help)
        {
            _printHelp();
            return 1;
        }
        
        const std::filesystem::path parentPath = std::filesystem::path(_exe).parent_path();
        const std::filesystem::path inputPath(_args.input);
        const std::filesystem::path outputPath(_args.output);
        const auto outputSplit = splitFileNameNumber(outputPath.stem().string());
        const int outputStartFrame = atoi(outputSplit.second.c_str());
        const size_t outputNumberPadding = getNumberPadding(outputSplit.second);

        // Open the timeline.
        _timelineWrapper = std::make_shared<TimelineWrapper>(inputPath);

        // Get time values.
        const OTIO_NS::TimeRange& timeRange = _timelineWrapper->getTimeRange();
        const OTIO_NS::RationalTime timeInc(1.0, timeRange.duration().rate());
        const int frames = timeRange.duration().value();
        
        // Create the image graph.
        std::shared_ptr<MessageLog> log;
        if (_options.verbose)
        {
            log = std::make_shared<MessageLog>();
        }
        ImageGraphOptions imageGraphOptions;
        imageGraphOptions.log = log;
        _graph = std::make_shared<ImageGraph>(
            inputPath.parent_path(),
            _timelineWrapper,
            imageGraphOptions);
        const IMATH_NAMESPACE::V2d imageSize = _graph->getImageSize();

        // Create the image host.
        std::vector<std::filesystem::path> searchPath;
        searchPath.push_back(parentPath);
#if defined(_WINDOWS)
        searchPath.push_back(parentPath / ".." / ".." / "..");
#else // _WINDOWS
        searchPath.push_back(parentPath / ".." / "..");
#endif // _WINDOWS
        ImageEffectHostOptions imageHostOptions;
        imageHostOptions.log = log;
        _host = std::make_shared<ImageEffectHost>(
            searchPath,
            imageHostOptions);

        // Initialize the filmstrip.
        OIIO::ImageBuf filmstripBuf;
        const int thumbnailWidth = 360;
        const int thumbnailSpacing = 0;
        IMATH_NAMESPACE::V2d thumbnailSize;
        if (imageSize.x > 0 && imageSize.y > 0)
        {
            thumbnailSize = IMATH_NAMESPACE::V2d(
                thumbnailWidth,
                thumbnailWidth / static_cast<float>(imageSize.x / static_cast<float>(imageSize.y)));
            const IMATH_NAMESPACE::V2d filmstripSize(
                thumbnailSize.x * frames + thumbnailSpacing * (frames - 1),
                thumbnailSize.y);
            filmstripBuf = OIIO::ImageBufAlgo::fill(
                { 0.F, 0.F, 0.F, 0.F },
                OIIO::ROI(0, filmstripSize.x, 0, filmstripSize.y, 0, 1, 0, 4));
        }

        // Render the timeline frames.
        int filmstripX = 0;
        for (OTIO_NS::RationalTime time = timeRange.start_time();
            time <= timeRange.end_time_inclusive();
            time += timeInc)
        {
            std::cout << (time - timeRange.start_time()).value() << "/" <<
                timeRange.duration().value() << std::endl;

            if (auto node = _graph->exec(_host, time))
            {
                // Execute the graph.
                const auto buf = node->exec();

                // Append the image.
                const auto thumbnailBuf = OIIO::ImageBufAlgo::resize(
                    buf,
                    "",
                    0.0,
                    OIIO::ROI(0, thumbnailSize.x, 0, thumbnailSize.y, 0, 1, 0, 4));
                OIIO::ImageBufAlgo::paste(
                    filmstripBuf,
                    filmstripX,
                    0,
                    0,
                    0,
                    thumbnailBuf);
                filmstripX += thumbnailSize.x + thumbnailSpacing;
            }
        }

        // Write the image.
        filmstripBuf.write(outputPath.string());
    
        return 0;
    }

    void App::_printHelp()
    {
        std::cout << "Usage:" << std::endl;
        std::cout << std::endl;
        std::cout << "    toucan-filmstrip (input) (output) [options...]" << std::endl;
        std::cout << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << std::endl;
        for (const auto& arg : _args.list)
        {
            std::cout << "    " << arg->getName() << " - " << arg->getHelp() << std::endl;
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << std::endl;
        for (const auto& option : _options.list)
        {
            for (const auto& line : option->getHelp())
            {
                std::cout << "    " << line << std::endl;
            }
            std::cout << std::endl;
        }        
    }
}

