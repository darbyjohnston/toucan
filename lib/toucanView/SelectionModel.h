// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

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
    //! Selection type.
    enum class SelectionType
    {
        All,
        Tracks,
        Clips
    };

    //! Selection model.
    class SelectionModel : public std::enable_shared_from_this<SelectionModel>
    {
    public:
        SelectionModel();

        virtual ~SelectionModel();

        //! Get the selection.
        const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& getSelection() const;

        //! Observe the selection.
        std::shared_ptr<dtk::IObservableList<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > > observeSelection() const;

        //! Set the seldction.
        void setSelection(const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >&);

        //! Select all of the given type.
        void selectAll(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&,
            SelectionType = SelectionType::All);

        //! Clear the selection.
        void clearSelection();

        //! Invert the selection.
        void invertSelection(const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&);

    private:
        std::shared_ptr<dtk::ObservableList<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > > _selection;
    };
}
