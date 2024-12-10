// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "OTIOZ.h"

namespace toucan
{
    std::unique_ptr<OIIO::Filesystem::IOMemReader> getMemoryReader(const MemoryReference& ref)
    {
        return ref.isValid() ?
            std::make_unique<OIIO::Filesystem::IOMemReader>(ref.getData(), ref.getSize()) :
            nullptr;
    }
}
