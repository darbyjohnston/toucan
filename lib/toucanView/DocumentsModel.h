// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "Document.h"

#include <dtk/ui/RecentFilesModel.h>
#include <dtk/ui/Settings.h>
#include <dtk/core/Context.h>
#include <dtk/core/ObservableList.h>
#include <dtk/core/ObservableValue.h>

namespace toucan
{
    class Document;
    class ImageEffectHost;

    //! Documents model.
    class DocumentsModel : public std::enable_shared_from_this<DocumentsModel>
    {
    public:
        DocumentsModel(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<dtk::Settings>&,
            const std::shared_ptr<ImageEffectHost>&);

        virtual ~DocumentsModel();

        //! Open a document.
        void open(const std::filesystem::path&);

        //! Close the current document.
        void close();

        //! Close a document.
        void close(int);

        //! Close all documents.
        void closeAll();

        //! Observe the documents.
        std::shared_ptr<dtk::IObservableList<std::shared_ptr<Document> > > observeDocuments() const;

        //! Observe when a document is added.
        std::shared_ptr<dtk::IObservableValue<int> > observeAdd() const;

        //! Observe when a document is removed.
        std::shared_ptr<dtk::IObservableValue<int> > observeRemove() const;

        //! Observe the current document.
        std::shared_ptr<dtk::IObservableValue<std::shared_ptr<Document> > > observeCurrent() const;

        //! Observe the current document index.
        std::shared_ptr<dtk::IObservableValue<int> > observeCurrentIndex() const;

        //! Set the current document index.
        void setCurrentIndex(int);

        //! Go to the next document.
        void next();

        //! Go to the previous document.
        void prev();

        //! Get the recent files model.
        const std::shared_ptr<dtk::RecentFilesModel>& getRecentFilesModel() const;

    private:
        std::weak_ptr<dtk::Context> _context;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<dtk::ObservableList<std::shared_ptr<Document> > > _documents;
        std::shared_ptr<dtk::ObservableValue<int> > _add;
        std::shared_ptr<dtk::ObservableValue<int> > _remove;
        std::shared_ptr<dtk::ObservableValue<std::shared_ptr<Document> > > _current;
        std::shared_ptr<dtk::ObservableValue<int> > _currentIndex;
        std::shared_ptr<dtk::RecentFilesModel> _recentFilesModel;
    };
}

