// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanUtil/MemoryMap.h>

#include <OpenImageIO/filesystem.h>

#include <filesystem>
#include <map>
#include <string>

namespace toucan
{
    //! Map URLs to memory references.
    typedef std::map<std::string, MemoryReference> MemoryReferences;

    //! Get an OIIO memory reader for a memory reference.
    std::unique_ptr<OIIO::Filesystem::IOMemReader> getMemoryReader(const MemoryReference&);
}
