// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "PropertySetTest.h"

#include <toucan/PropertySet.h>

#include <cassert>

namespace toucan
{
    void propertySetTest()
    {
        {
            int a = 1;
            PropertySet p;
            p.setPointer("p", 0, &a);
            int* b = nullptr;
            p.getPointer("p", 0, (void**)&b);
            assert(b);
            assert(b == &a);
        }
        {
            std::string a = "a";
            PropertySet p;
            p.setString("s", 0, a.c_str());
            char* b = nullptr;
            p.getString("s", 0, &b);
            assert(b);
            assert(b == a);
        }
        {
            double a = 1.0;
            PropertySet p;
            p.setDouble("d", 0, a);
            double b = 0.0;
            p.getDouble("d", 0, &b);
            assert(b);
            assert(b == a);
        }
        {
            int a = 1;
            PropertySet p;
            p.setInt("i", 0, a);
            int b = 0.0;
            p.getInt("i", 0, &b);
            assert(b);
            assert(b == a);
        }
        {
            int v0 = 0;
            int v1 = 1;
            void* a[] = { &v0, &v1 };
            PropertySet p;
            p.setPointerN("p", 2, a);
            void* b[2] = { nullptr, nullptr };
            p.getPointerN("p", 2, b);
            assert(b);
            assert(b[0] == a[0]);
            assert(b[1] == a[1]);
        }
        {
            std::string v[2] = { "v0", "v1" };
            const char* a[] = { v[0].c_str(), v[1].c_str() };
            PropertySet p;
            p.setStringN("s", 2, a);
            char* b[2] = { nullptr, nullptr };
            p.getStringN("s", 2, b);
            assert(b);
            assert(b[0] == v[0]);
            assert(b[1] == v[1]);
        }
        {
            double a[2] = { 1.0, 2.0 };
            PropertySet p;
            p.setDoubleN("d", 2, a);
            double b[2] = { 0.0, 0.0 };
            p.getDoubleN("d", 2, b);
            assert(b[0] == a[0]);
            assert(b[1] == a[1]);
        }
        {
            int a[2] = { 1, 2 };
            PropertySet p;
            p.setIntN("i", 2, a);
            int b[2] = { 0, 0 };
            p.getIntN("i", 2, b);
            assert(b[0] == a[0]);
            assert(b[1] == a[1]);
        }
        {
            PropertySet p;
            int dim = 0;
            p.getDimension("i", &dim);
            assert(0 == dim);
            p.setInt("i", 0, 1);
            dim = 0;
            p.getDimension("i", &dim);
            assert(1 == dim);
            p.setInt("i", 1, 2);
            dim = 0;
            p.getDimension("i", &dim);
            assert(2 == dim);
        }
        {
            PropertySet p;
            OfxStatus status = p.reset("i");
            assert(status != kOfxStatOK);
            p.setInt("i", 0, 1);
            status = p.reset("i");
            assert(kOfxStatOK == status);
        }
    }
}
