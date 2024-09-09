// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "DocumentsModel.h"

#include "PlaybackModel.h"

#include <dtk/core/Math.h>

namespace toucan
{
    DocumentsModel::DocumentsModel(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<ImageEffectHost>& host) :
        _context(context),
        _host(host)
    {
        _documents = dtk::ObservableList< std::shared_ptr<Document> >::create();
        _add = dtk::ObservableValue< std::shared_ptr<Document> >::create(nullptr);
        _remove = dtk::ObservableValue< std::shared_ptr<Document> >::create(nullptr);
        _current = dtk::ObservableValue< std::shared_ptr<Document> >::create(nullptr);
        _currentIndex = dtk::ObservableValue<int>::create(-1);
    }

    DocumentsModel::~DocumentsModel()
    {}

    void DocumentsModel::open(const std::filesystem::path& path)
    {
        if (auto context = _context.lock())
        {
            auto documents = _documents->get();
            auto document = std::make_shared<Document>(context, _host, path);
            documents.push_back(document);
            _documents->setIfChanged(documents);
            _add->setAlways(document);
            const int index = documents.size() - 1;
            _current->setIfChanged(documents[index]);
            _currentIndex->setIfChanged(index);
        }
    }

    void DocumentsModel::close()
    {
        auto documents = _documents->get();
        int current = _currentIndex->get();
        if (current >= 0 && current < documents.size())
        {
            auto document = *(documents.begin() + current);
            documents.erase(documents.begin() + current);
            _documents->setIfChanged(documents);
            _remove->setAlways(document);
            current = std::min(current, static_cast<int>(documents.size()) - 1);
            _current->setAlways(
                (current >= 0 && current < documents.size()) ?
                documents[current] :
                nullptr);
            _currentIndex->setAlways(current);
        }
    }

    void DocumentsModel::closeAll()
    {
        auto documents = _documents->get();
        _documents->setIfChanged({});
        for (const auto& document : documents)
        {
            _remove->setAlways(document);
        }
        _current->setIfChanged(nullptr);
        _currentIndex->setIfChanged(-1);
    }

    std::shared_ptr<dtk::IObservableList<std::shared_ptr<Document> > > DocumentsModel::observeDocuments() const
    {
        return _documents;
    }

    std::shared_ptr<dtk::IObservableValue<std::shared_ptr<Document> > > DocumentsModel::observeAdd() const
    {
        return _add;
    }

    std::shared_ptr<dtk::IObservableValue<std::shared_ptr<Document> > > DocumentsModel::observeRemove() const
    {
        return _remove;
    }

    std::shared_ptr<dtk::IObservableValue<std::shared_ptr<Document> > > DocumentsModel::observeCurrent() const
    {
        return _current;
    }

    std::shared_ptr<dtk::IObservableValue<int> > DocumentsModel::observeCurrentIndex() const
    {
        return _currentIndex;
    }

    void DocumentsModel::setCurrentIndex(int value)
    {
        const auto& documents = _documents->get();
        const int index = dtk::clamp(value, 0, static_cast<int>(documents.size()) - 1);
        _current->setIfChanged(documents[index]);
        _currentIndex->setIfChanged(index);
    }

    void DocumentsModel::next()
    {
        const auto& documents = _documents->get();
        if (!documents.empty())
        {
            int index = _currentIndex->get() + 1;
            if (index >= documents.size())
            {
                index = 0;
            }
            setCurrentIndex(index);
        }
    }

    void DocumentsModel::prev()
    {
        const auto& documents = _documents->get();
        if (!documents.empty())
        {
            int index = _currentIndex->get() - 1;
            if (index < 0)
            {
                index = static_cast<int>(documents.size()) - 1;
            }
            setCurrentIndex(index);
        }
    }
}
