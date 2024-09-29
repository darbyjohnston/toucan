// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucanEdit/IItem.h>

namespace toucan
{
    enum class Find
    {
        Recurse,
        Shallow
    };

    class IContainer : public IItem
    {
    public:
        IContainer(
            const std::string& name = std::string(),
            const OTIO_NS::TimeRange& range = OTIO_NS::TimeRange(),
            const OTIO_NS::AnyDictionary& metadata = OTIO_NS::AnyDictionary());

        virtual ~IContainer() = 0;

        const std::vector<std::shared_ptr<IItem> >& getChildren() const;
        void addChild(const std::shared_ptr<IItem>&);
        void removeChild(const std::shared_ptr<IItem>&);

        virtual OTIO_NS::TimeRange getChildRange(const std::shared_ptr<IItem>&);

        template<typename T>
        std::shared_ptr<T> find(Find find = Find::Recurse) const;
        template<typename T>
        std::vector<std::shared_ptr<T> > findAll(Find find = Find::Recurse) const;

    private:
        template<typename T>
        void _findAll(Find, std::vector<std::shared_ptr<T> >&) const;

        std::vector<std::shared_ptr<IItem> > _children;
    };
}

#include "IContainerInline.h"

