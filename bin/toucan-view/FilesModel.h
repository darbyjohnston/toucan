// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtkCore/Context.h>
#include <dtkCore/ObservableList.h>
#include <dtkCore/ObservableValue.h>

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
        FilesModel(const std::shared_ptr<dtk::core::Context>&);

        virtual ~FilesModel();

        void open(const std::filesystem::path&);

        void close();

        std::shared_ptr<dtk::core::IObservableList<File> > observeFiles() const;

        std::shared_ptr<dtk::core::IObservableValue<int> > observeCurrent() const;

    private:
        std::weak_ptr<dtk::core::Context> _context;
        std::shared_ptr<dtk::core::ObservableList<File> > _files;
        std::shared_ptr<dtk::core::ObservableValue<int> > _current;
    };
}
