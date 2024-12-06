// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "File.h"

#include <dtk/ui/RecentFilesModel.h>
#include <dtk/core/Context.h>
#include <dtk/core/ObservableList.h>
#include <dtk/core/ObservableValue.h>

namespace toucan
{
    class File;
    class ImageEffectHost;

    //! Timeline files model.
    class FilesModel : public std::enable_shared_from_this<FilesModel>
    {
    public:
        FilesModel(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<ImageEffectHost>&);

        virtual ~FilesModel();

        //! Open a file.
        void open(const std::filesystem::path&);

        //! Close the current file.
        void close();

        //! Close a file.
        void close(int);

        //! Close all files.
        void closeAll();

        //! Observe the files.
        std::shared_ptr<dtk::IObservableList<std::shared_ptr<File> > > observeFiles() const;

        //! Observe when a file is added.
        std::shared_ptr<dtk::IObservableValue<int> > observeAdd() const;

        //! Observe when a file is removed.
        std::shared_ptr<dtk::IObservableValue<int> > observeRemove() const;

        //! Observe the current file.
        std::shared_ptr<dtk::IObservableValue<std::shared_ptr<File> > > observeCurrent() const;

        //! Observe the current file index.
        std::shared_ptr<dtk::IObservableValue<int> > observeCurrentIndex() const;

        //! Set the current file index.
        void setCurrentIndex(int);

        //! Go to the next file.
        void next();

        //! Go to the previous file.
        void prev();

        //! Get the recent files model.
        const std::shared_ptr<dtk::RecentFilesModel>& getRecentFilesModel() const;

    private:
        std::weak_ptr<dtk::Context> _context;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<dtk::ObservableList<std::shared_ptr<File> > > _files;
        std::shared_ptr<dtk::ObservableValue<int> > _add;
        std::shared_ptr<dtk::ObservableValue<int> > _remove;
        std::shared_ptr<dtk::ObservableValue<std::shared_ptr<File> > > _current;
        std::shared_ptr<dtk::ObservableValue<int> > _currentIndex;
        std::shared_ptr<dtk::RecentFilesModel> _recentFilesModel;
    };
}

