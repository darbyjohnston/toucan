// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "PropertySet.h"

namespace toucan
{
    OfxStatus PropertySet::setPointer(const char* property, int index, void* value)
    {
        auto& v = _p[property];
        if (index >= v.size())
        {
            v.resize(index + 1);
        }
        v[index] = value;
        return kOfxStatOK;
    }

    OfxStatus PropertySet::setString(const char* property, int index, const char* value)
    {
        auto& v = _s[property];
        if (index >= v.size())
        {
            v.resize(index + 1);
        }
        v[index] = value;
        return kOfxStatOK;
    }

    OfxStatus PropertySet::setDouble(const char* property, int index, double value)
    {
        auto& v = _d[property];
        if (index >= v.size())
        {
            v.resize(index + 1);
        }
        v[index] = value;
        return kOfxStatOK;
    }

    OfxStatus PropertySet::setInt(const char* property, int index, int value)
    {
        auto& v = _i[property];
        if (index >= v.size())
        {
            v.resize(index + 1);
        }
        v[index] = value;
        return kOfxStatOK;
    }

    OfxStatus PropertySet::setPointerN(const char* property, int count, void* const* value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::setStringN(const char* property, int count, const char* const* value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::setDoubleN(const char* property, int count, const double* value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::setIntN(const char* property, int count, const int* value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::getPointer(const char* property, int index, void** value)
    {
        auto p = _p.find(property);
        if (p != _p.end())
        {
            const auto& v = p->second;
            if (index < v.size())
            {
                *value = v[index];
                return kOfxStatOK;
            }
        }
        return kOfxStatFailed;
    }

    OfxStatus PropertySet::getString(const char* property, int index, char** value)
    {
        auto s = _s.find(property);
        if (s != _s.end())
        {
            const auto& v = s->second;
            if (index < v.size())
            {
                const auto& string = v[index];
                const size_t size = string.size();
                _sBuf.resize(size + 1);
                memcpy(_sBuf.data(), string.c_str(), size);
                _sBuf[size] = 0;
                *value = _sBuf.data();
                return kOfxStatOK;
            }
        }
        return kOfxStatFailed;
    }

    OfxStatus PropertySet::getDouble(const char* property, int index, double* value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::getInt(const char* property, int index, int* value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::getPointerN(const char* property, int count, void** value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::getStringN(const char* property, int count, char** value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::getDoubleN(const char* property, int count, double* value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::getIntN(const char* property, int count, int* value)
    {
        return kOfxStatOK;
    }

    OfxStatus PropertySet::reset(const char* property)
    {
        OfxStatus out = kOfxStatFailed;
        auto p = _p.find(property);
        if (p != _p.end())
        {
            _p.erase(p);
            out = kOfxStatOK;
        }
        auto s = _s.find(property);
        if (s != _s.end())
        {
            _s.erase(s);
            out = kOfxStatOK;
        }
        auto d = _d.find(property);
        if (d != _d.end())
        {
            _d.erase(d);
            out = kOfxStatOK;
        }
        auto i = _i.find(property);
        if (i != _i.end())
        {
            _i.erase(i);
            out = kOfxStatOK;
        }
        return out;
    }

    OfxStatus PropertySet::getDimension(const char* property, int* count)
    {
        OfxStatus out = kOfxStatFailed;
        auto p = _p.find(property);
        if (p != _p.end())
        {
            *count = p->second.size();
            out = kOfxStatOK;
        }
        auto s = _s.find(property);
        if (s != _s.end())
        {
            *count = s->second.size();
            out = kOfxStatOK;
        }
        auto d = _d.find(property);
        if (d != _d.end())
        {
            *count = d->second.size();
            out = kOfxStatOK;
        }
        auto i = _i.find(property);
        if (i != _i.end())
        {
            *count = i->second.size();
            out = kOfxStatOK;
        }
        return out;
    }

    OfxStatus PropertySet::setPointer(OfxPropertySetHandle handle, const char* property, int index, void* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->setPointer(property, index, value);
    }

    OfxStatus PropertySet::setString(OfxPropertySetHandle handle, const char* property, int index, const char* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->setString(property, index, value);
    }

    OfxStatus PropertySet::setDouble(OfxPropertySetHandle handle, const char* property, int index, double value)
    {
        return reinterpret_cast<PropertySet*>(handle)->setDouble(property, index, value);
    }

    OfxStatus PropertySet::setInt(OfxPropertySetHandle handle, const char* property, int index, int value)
    {
        return reinterpret_cast<PropertySet*>(handle)->setInt(property, index, value);
    }

    OfxStatus PropertySet::setPointerN(OfxPropertySetHandle handle, const char* property, int count, void* const* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->setPointerN(property, count, value);
    }

    OfxStatus PropertySet::setStringN(OfxPropertySetHandle handle, const char* property, int count, const char* const* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->setStringN(property, count, value);
    }

    OfxStatus PropertySet::setDoubleN(OfxPropertySetHandle handle, const char* property, int count, const double* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->setDoubleN(property, count, value);
    }

    OfxStatus PropertySet::setIntN(OfxPropertySetHandle handle, const char* property, int count, const int* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->setIntN(property, count, value);
    }

    OfxStatus PropertySet::getPointer(OfxPropertySetHandle handle, const char* property, int index, void** value)
    {
        return reinterpret_cast<PropertySet*>(handle)->getPointer(property, index, value);
    }

    OfxStatus PropertySet::getString(OfxPropertySetHandle handle, const char* property, int index, char** value)
    {
        return reinterpret_cast<PropertySet*>(handle)->getString(property, index, value);
    }

    OfxStatus PropertySet::getDouble(OfxPropertySetHandle handle, const char* property, int index, double* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->getDouble(property, index, value);
    }

    OfxStatus PropertySet::getInt(OfxPropertySetHandle handle, const char* property, int index, int* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->getInt(property, index, value);
    }

    OfxStatus PropertySet::getPointerN(OfxPropertySetHandle handle, const char* property, int count, void** value)
    {
        return reinterpret_cast<PropertySet*>(handle)->getPointerN(property, count, value);
    }

    OfxStatus PropertySet::getStringN(OfxPropertySetHandle handle, const char* property, int count, char** value)
    {
        return reinterpret_cast<PropertySet*>(handle)->getStringN(property, count, value);
    }

    OfxStatus PropertySet::getDoubleN(OfxPropertySetHandle handle, const char* property, int count, double* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->getDoubleN(property, count, value);
    }

    OfxStatus PropertySet::getIntN(OfxPropertySetHandle handle, const char* property, int count, int* value)
    {
        return reinterpret_cast<PropertySet*>(handle)->getIntN(property, count, value);
    }

    OfxStatus PropertySet::reset(OfxPropertySetHandle handle, const char* property)
    {
        return reinterpret_cast<PropertySet*>(handle)->reset(property);
    }

    OfxStatus PropertySet::getDimension(OfxPropertySetHandle handle, const char* property, int* count)
    {
        return reinterpret_cast<PropertySet*>(handle)->getDimension(property, count);
    }
}
