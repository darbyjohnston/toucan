// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "SelectionModel.h"

#include "Timeline.h"

#include <set>

namespace toucan
{
    SelectionModel::SelectionModel()
    {
        _selection = dtk::ObservableList<std::shared_ptr<IItem> >::create();
    }

    SelectionModel::~SelectionModel()
    {}

    const std::vector<std::shared_ptr<IItem> >& SelectionModel::getSelection() const
    {
        return _selection->get();
    }

    std::shared_ptr<dtk::IObservableList<std::shared_ptr<IItem> > > SelectionModel::observeSelection() const
    {
        return _selection;
    }

    void SelectionModel::setSelection(const std::vector<std::shared_ptr<IItem> >& selection)
    {
        std::set<std::shared_ptr<IItem> > set;
        set.insert(selection.begin(), selection.end());
        std::vector<std::shared_ptr<IItem> > tmp;
        tmp.insert(tmp.end(), set.begin(), set.end());
        _selection->setIfChanged(tmp);
    }

    void SelectionModel::selectAll(const std::shared_ptr<Timeline>& timeline)
    {
        std::vector<std::shared_ptr<IItem> > items;
        //for (auto& item : timeline->find_children<OTIO_NS::Item>())
        //{
        //    items.push_back(item);
        //}
        _selection->setIfChanged(items);
    }

    void SelectionModel::clearSelection()
    {
        _selection->setIfChanged({});
    }

    void SelectionModel::invertSelection(const std::shared_ptr<Timeline>& timeline)
    {
        std::vector<std::shared_ptr<IItem> > items;
        //for (auto& item : timeline->find_children<OTIO_NS::Item>())
        //{
        //    items.push_back(item);
        //}
        const auto& selection = _selection->get();
        auto i = items.begin();
        while (i != items.end())
        {
            auto j = std::find(selection.begin(), selection.end(), *i);
            if (j != selection.end())
            {
                i = items.erase(i);
            }
            else
            {
                ++i;
            }
        }
        _selection->setIfChanged(items);
    }
}
