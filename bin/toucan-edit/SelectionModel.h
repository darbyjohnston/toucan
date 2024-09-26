// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/core/ObservableList.h>

namespace toucan
{
    class IItem;
    class Timeline;

    class SelectionModel : public std::enable_shared_from_this<SelectionModel>
    {
    public:
        SelectionModel();

        virtual ~SelectionModel();

        const std::vector<std::shared_ptr<IItem> >& getSelection() const;
        std::shared_ptr<dtk::IObservableList<std::shared_ptr<IItem> > > observeSelection() const;
        void setSelection(const std::vector<std::shared_ptr<IItem> >&);

        void selectAll(const std::shared_ptr<Timeline>&);
        void clearSelection();
        void invertSelection(const std::shared_ptr<Timeline>&);

    private:
        std::shared_ptr<dtk::ObservableList<std::shared_ptr<IItem> > > _selection;
    };
}
