// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "IItem.h"

#include <vector>

namespace toucan
{
    class IContainer : public IItem
    {
    public:
        IContainer();

        virtual ~IContainer() = 0;

        const std::vector<std::shared_ptr<IItem> >& getChildren() const;
        void addChild(const std::shared_ptr<IItem>&);
        void removeChild(const std::shared_ptr<IItem>&);

    private:
        std::vector<std::shared_ptr<IItem> > _children;
    };
}

