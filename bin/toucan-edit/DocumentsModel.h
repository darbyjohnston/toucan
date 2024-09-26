// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Document.h"

#include <toucan/ImageEffectHost.h>

#include <dtk/core/Context.h>
#include <dtk/core/ObservableList.h>
#include <dtk/core/ObservableValue.h>

namespace toucan
{
    class DocumentsModel : public std::enable_shared_from_this<DocumentsModel>
    {
    public:
        DocumentsModel(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<ImageEffectHost>&);

        virtual ~DocumentsModel();

        void open(const std::filesystem::path&);
        void close();
        void close(int);
        void closeAll();

        std::shared_ptr<dtk::IObservableList<std::shared_ptr<Document> > > observeDocuments() const;
        std::shared_ptr<dtk::IObservableValue<int> > observeAdd() const;
        std::shared_ptr<dtk::IObservableValue<int> > observeRemove() const;
        std::shared_ptr<dtk::IObservableValue<std::shared_ptr<Document> > > observeCurrent() const;
        std::shared_ptr<dtk::IObservableValue<int> > observeCurrentIndex() const;
        void setCurrentIndex(int);
        void next();
        void prev();

    private:
        std::weak_ptr<dtk::Context> _context;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<dtk::ObservableList<std::shared_ptr<Document> > > _documents;
        std::shared_ptr<dtk::ObservableValue<int> > _add;
        std::shared_ptr<dtk::ObservableValue<int> > _remove;
        std::shared_ptr<dtk::ObservableValue<std::shared_ptr<Document> > > _current;
        std::shared_ptr<dtk::ObservableValue<int> > _currentIndex;
    };
}

