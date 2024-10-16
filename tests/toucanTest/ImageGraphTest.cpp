// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageGraphTest.h"

#include <toucan/ImageGraph.h>
#include <toucan/Util.h>

#include <sstream>

namespace toucan
{
    void imageGraphTest(
        const std::filesystem::path& path,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        std::cout << "imageGraphTest" << std::endl;
        const std::vector<std::string> otioFiles =
        {
            "ColorSpace",
            "CompositeTracks",
            "Draw",
            "Filter",
            "Gap",
            "Generator",
            "LinearTimeWarp",
            "MultipleEffects",
            "Transform",
            "Transition",
            "Transition2",
            "TransitionWipe"
        };
        for (const auto& otioFile : otioFiles)
        {
            // Open the timeline.
            const std::filesystem::path timelinePath = path / (otioFile + ".otio");
            OTIO_NS::ErrorStatus errorStatus;
            auto timeline = OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>(
                dynamic_cast<OTIO_NS::Timeline*>(OTIO_NS::Timeline::from_json_file(timelinePath.string(), &errorStatus)));
            if (!timeline)
            {
                std::cout << "ERROR: " << timelinePath.string() << ": " << errorStatus.full_description << std::endl;
                continue;
            }

            // Compute time values.
            const OTIO_NS::RationalTime startTime = timeline->global_start_time().has_value() ?
                timeline->global_start_time().value() :
                OTIO_NS::RationalTime(0.0, timeline->duration().rate());
            const OTIO_NS::TimeRange timeRange(startTime, timeline->duration());
            const OTIO_NS::RationalTime timeInc(1.0, timeline->duration().rate());

            // Render the timeline frames.
            const auto graph = std::make_shared<ImageGraph>(path, timeline);
            for (OTIO_NS::RationalTime time = startTime;
                time <= timeRange.end_time_inclusive();
                time += timeInc)
            {
                std::cout << "  " << otioFile << ": " << time.value() << "/" <<
                    timeRange.duration().value() << std::endl;
                if (auto op = graph->exec(host, time))
                {
                    // Execute the image operation graph.
                    const auto buf = op->exec();

                    // Save the image.
                    const std::filesystem::path imagePath = getSequenceFrame(
                        std::filesystem::path(),
                        "imageGraphTest_" + otioFile + ".",
                        time.to_frames(),
                        6,
                        ".png");
                    buf.write(imagePath.string());
                }
            }
        }
    }
}
