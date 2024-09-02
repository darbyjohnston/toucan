// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

namespace toucan
{
    enum FrameAction
    {
        Start,
        Prev,
        Next,
        End
    };

    enum class Playback
    {
        Stop,
        Forward,
        Reverse
    };
}
