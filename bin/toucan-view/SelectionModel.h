// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/core/ObservableList.h>

#include <opentimelineio/timeline.h>

namespace OTIO_NS
{
    //! \todo Move to OTIO?
    bool operator == (
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&);
    bool operator < (
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&);
}

namespace toucan
{
    class SelectionModel : public std::enable_shared_from_this<SelectionModel>
    {
    public:
        SelectionModel();

        virtual ~SelectionModel();

        const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& getSelection() const;
        std::shared_ptr<dtk::IObservableList<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > > observeSelection() const;
        void setSelection(const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >&);

        void selectAll(const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&);
        void clearSelection();
        void invertSelection(const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&);

    private:
        std::shared_ptr<dtk::ObservableList<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > > _selection;
    };
}
