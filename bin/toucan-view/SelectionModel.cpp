// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "SelectionModel.h"

#include <set>

namespace OTIO_NS
{
    bool operator == (
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& a,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& b)
    {
        return a.value == b.value;
    }

    bool operator < (
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& a,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& b)
    {
        return a.value < b.value;
    }
}

namespace toucan
{
    SelectionModel::SelectionModel()
    {
        _selection = dtk::ObservableList<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >::create();
    }

    SelectionModel::~SelectionModel()
    {}

    const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& SelectionModel::getSelection() const
    {
        return _selection->get();
    }

    std::shared_ptr<dtk::IObservableList<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > > SelectionModel::observeSelection() const
    {
        return _selection;
    }

    void SelectionModel::setSelection(const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& selection)
    {
        std::set<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > set;
        set.insert(selection.begin(), selection.end());
        std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> > tmp;
        tmp.insert(tmp.end(), set.begin(), set.end());
        _selection->setIfChanged(tmp);
    }
}
