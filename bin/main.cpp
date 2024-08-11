// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include <toucan/Init.h>
#include <toucan/TimelineTraverse.h>
#include <toucan/Util.h>

#include <iostream>

using namespace toucan;

int main(int argc, char** argv)
{
    // Command line arguments.
    if (argc != 3)
    {
        std::cout << "Usage: toucan-render (input.otio) (output.0001.tif)" << std::endl;
        return 1;
    }
    const std::filesystem::path inputPath(argv[1]);
    const std::filesystem::path outputPath(argv[2]);
    const auto outputSplit = splitFileNameNumber(outputPath.stem().string());
    const int outputStartFrame = atoi(outputSplit.second.c_str());
    const size_t outputNumberPadding = getNumberPadding(outputSplit.second);

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

    // Traverse each frame of the timeline.
    auto traverse = std::make_shared<TimelineTraverse>(inputPath.parent_path(), timeline);
    for (OTIO_NS::RationalTime time = startTime;
        time <= timeRange.end_time_inclusive();
        time += timeInc)
    {
        std::cout << time.value() << "/" << timeRange.duration().value() << std::endl;
        if (auto op = traverse->exec(time))
        {
            // Execute the image operation graph.
            const auto buf = op->exec(time);

            // Save the image.
            std::filesystem::path imagePath = outputPath.parent_path() / outputSplit.first;
            std::stringstream ss;
            ss << imagePath.string() <<
                std::setw(outputNumberPadding) << std::setfill('0') << (outputStartFrame + time.to_frames()) <<
                std::setw(0) <<
                outputPath.extension().string();
            buf.write(ss.str());
        }
    }

    return 0;
}
