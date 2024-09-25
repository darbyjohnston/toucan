// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "IContainer.h"

#include <vector>

namespace toucan
{
    class Stack : public IContainer
    {
    public:
        Stack();

        virtual ~Stack();
    };
}

