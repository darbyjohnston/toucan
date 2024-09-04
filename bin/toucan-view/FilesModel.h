// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/core/Context.h>
#include <dtk/core/ObservableList.h>
#include <dtk/core/ObservableValue.h>

#include <opentimelineio/timeline.h>

#include <filesystem>
#include <list>

namespace toucan
{
    struct File
    {
        std::filesystem::path path;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> timeline;

        bool operator == (const File&) const;
    };

    class FilesModel : public std::enable_shared_from_this<FilesModel>
    {
    public:
        FilesModel(const std::shared_ptr<dtk::Context>&);

        virtual ~FilesModel();

        void open(const std::filesystem::path&);
        void close();

        std::shared_ptr<dtk::IObservableList<File> > observeFiles() const;
        std::shared_ptr<dtk::IObservableValue<int> > observeCurrentIndex() const;
        std::shared_ptr<dtk::IObservableValue<File> > observeCurrentFile() const;
        void setCurrentIndex(int);

        void next();
        void prev();

    private:
        std::weak_ptr<dtk::Context> _context;
        std::shared_ptr<dtk::ObservableList<File> > _files;
        std::shared_ptr<dtk::ObservableValue<int> > _currentIndex;
        std::shared_ptr<dtk::ObservableValue<File> > _currentFile;
    };
}
