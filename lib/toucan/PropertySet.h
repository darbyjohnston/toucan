// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <OpenFX/ofxProperty.h>

#include <map>
#include <string>
#include <vector>

namespace toucan
{
    class PropertySet
    {
    public:
        OfxStatus setPointer(const char* property, int index, void* value);
        OfxStatus setString(const char* property, int index, const char* value);
        OfxStatus setDouble(const char* property, int index, double value);
        OfxStatus setInt(const char* property, int index, int value);
        OfxStatus setPointerN(const char* property, int count, void* const* value);
        OfxStatus setStringN(const char* property, int count, const char* const* value);
        OfxStatus setDoubleN(const char* property, int count, const double* value);
        OfxStatus setIntN(const char* property, int count, const int* value);
        OfxStatus getPointer(const char* property, int index, void** value);
        OfxStatus getString(const char* property, int index, char** value);
        OfxStatus getDouble(const char* property, int index, double* value);
        OfxStatus getInt(const char* property, int index, int* value);
        OfxStatus getPointerN(const char* property, int count, void** value);
        OfxStatus getStringN(const char* property, int count, char** value);
        OfxStatus getDoubleN(const char* property, int count, double* value);
        OfxStatus getIntN(const char* property, int count, int* value);
        OfxStatus reset(const char* property);
        OfxStatus getDimension(const char* property, int* count);

        static OfxStatus setPointer(OfxPropertySetHandle, const char* property, int index, void* value);
        static OfxStatus setString(OfxPropertySetHandle, const char* property, int index, const char* value);
        static OfxStatus setDouble(OfxPropertySetHandle, const char* property, int index, double value);
        static OfxStatus setInt(OfxPropertySetHandle, const char* property, int index, int value);
        static OfxStatus setPointerN(OfxPropertySetHandle, const char* property, int count, void* const* value);
        static OfxStatus setStringN(OfxPropertySetHandle, const char* property, int count, const char* const* value);
        static OfxStatus setDoubleN(OfxPropertySetHandle, const char* property, int count, const double* value);
        static OfxStatus setIntN(OfxPropertySetHandle, const char* property, int count, const int* value);
        static OfxStatus getPointer(OfxPropertySetHandle, const char* property, int index, void** value);
        static OfxStatus getString(OfxPropertySetHandle, const char* property, int index, char** value);
        static OfxStatus getDouble(OfxPropertySetHandle, const char* property, int index, double* value);
        static OfxStatus getInt(OfxPropertySetHandle, const char* property, int index, int* value);
        static OfxStatus getPointerN(OfxPropertySetHandle, const char* property, int count, void** value);
        static OfxStatus getStringN(OfxPropertySetHandle, const char* property, int count, char** value);
        static OfxStatus getDoubleN(OfxPropertySetHandle, const char* property, int count, double* value);
        static OfxStatus getIntN(OfxPropertySetHandle, const char* property, int count, int* value);
        static OfxStatus reset(OfxPropertySetHandle, const char* property);
        static OfxStatus getDimension(OfxPropertySetHandle, const char* property, int* count);

    private:
        std::map<std::string, std::vector<void*> > _p;
        std::map<std::string, std::vector<std::string> > _s;
        std::vector<char> _sBuf;
        std::map<std::string, std::vector<double> > _d;
        std::map<std::string, std::vector<int> > _i;
    };

}
