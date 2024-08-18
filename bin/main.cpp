// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include <toucan/ImageEffectHost.h>
#include <toucan/Init.h>
#include <toucan/TimelineGraph.h>
#include <toucan/Util.h>

#include <OpenImageIO/imagebufalgo.h>

using namespace toucan;

int main(int argc, char** argv)
{
    // Command line arguments.
    if (argc < 3)
    {
        std::cout << "Usage: toucan-render (input) (output) [options...]" << std::endl;
        std::cout << std::endl;
        std::cout << "* input - Input timeline .otio file" << std::endl;
        std::cout << "* output - Output image file (example: output.0001.tif)" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << std::endl;
        std::cout << "* -filmstrip - Render the timeline as thumbnails to a single image." << std::endl;
        std::cout << std::endl;
        return 1;
    }
    const std::filesystem::path parentPath = std::filesystem::path(argv[0]).parent_path();
    const std::filesystem::path inputPath(argv[1]);
    const std::filesystem::path outputPath(argv[2]);
    const auto outputSplit = splitFileNameNumber(outputPath.stem().string());
    const int outputStartFrame = atoi(outputSplit.second.c_str());
    const size_t outputNumberPadding = getNumberPadding(outputSplit.second);
    bool filmstrip = false;
    if (argc > 3)
    {
        filmstrip = std::string(argv[3]) == "-filmstrip";
    }

    // Initialize the toucan library.
    init();

    // Open the timeline.
    OTIO_NS::ErrorStatus errorStatus;
    auto timeline = OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>(
        dynamic_cast<OTIO_NS::Timeline*>(OTIO_NS::Timeline::from_json_file(inputPath.string(), &errorStatus)));
    if (!timeline)
    {
        std::cout << "ERROR: " << inputPath.string() << ": " << errorStatus.full_description << std::endl;
        return 1;
    }

    // Compute time values.
    const OTIO_NS::RationalTime startTime = timeline->global_start_time().has_value() ?
        timeline->global_start_time().value() :
        OTIO_NS::RationalTime(0.0, timeline->duration().rate());
    const OTIO_NS::TimeRange timeRange(startTime, timeline->duration());
    const OTIO_NS::RationalTime timeInc(1.0, timeline->duration().rate());
    const int frames = timeRange.duration().value();

    // Create the timeline graph.
    const auto graph = std::make_shared<TimelineGraph>(inputPath.parent_path(), timeline);
    const IMATH_NAMESPACE::V2d imageSize = graph->getImageSize();

    // Create the image effect host.
    std::vector<std::filesystem::path> searchPath;
    searchPath.push_back(parentPath);
#if defined(_WINDOWS)
    searchPath.push_back(parentPath / ".." / "..");
#else // _WINDOWS
    searchPath.push_back(parentPath / "..");
#endif // _WINDOWS
    auto host = std::make_shared<ImageEffectHost>(searchPath);

    // Initialize the filmstrip.
    OIIO::ImageBuf filmstripBuf;
    const int thumbnailWidth = 120;
    const int thumbnailSpacing = 20;
    IMATH_NAMESPACE::V2d thumbnailSize;
    if (filmstrip && imageSize.x > 0 && imageSize.y > 0)
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
    for (OTIO_NS::RationalTime time = startTime;
        time <= timeRange.end_time_inclusive();
        time += timeInc)
    {
        std::cout << time.value() << "/" << timeRange.duration().value() << std::endl;
        if (auto op = graph->exec(time))
        {
            // Execute the image operation graph.
            const auto buf = op->exec(time, host);

            // Save the image.
            if (!filmstrip)
            {
                std::filesystem::path imagePath = outputPath.parent_path() / outputSplit.first;
                std::stringstream ss;
                ss << imagePath.string() <<
                    std::setw(outputNumberPadding) << std::setfill('0') << (outputStartFrame + time.to_frames()) <<
                    std::setw(0) <<
                    outputPath.extension().string();
                buf.write(ss.str());
            }
            else
            {
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
    }
    if (filmstrip)
    {
        filmstripBuf.write(outputPath.string());
    }

    return 0;
}
