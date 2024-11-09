// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "MessageLog.h"

#include <iostream>

namespace toucan
{
    void MessageLog::log(
        const std::string& prefix,
        const std::string& message,
        MessageLogType type)
    {
        switch (type)
        {
        case MessageLogType::Info:
            std::cout << prefix << ": " << message << std::endl;
            break;
        case MessageLogType::Warning:
            std::cout << "Warning: " << message << std::endl;
            break;
        case MessageLogType::Error:
            std::cout << "ERROR: " << message << std::endl;
            break;
        }
    }
}
