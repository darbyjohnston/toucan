// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "FilesModel.h"

namespace toucan
{
    bool File::operator == (const File& other) const
    {
        return
            path == other.path && 
            timeline.value == other.timeline.value;
    }

    FilesModel::FilesModel(const std::shared_ptr<dtk::Context>& context) :
        _context(context)
    {
        _files = dtk::ObservableList<File>::create();
        _current = dtk::ObservableValue<int>::create(-1);
    }

    FilesModel::~FilesModel()
    {}

    void FilesModel::open(const std::filesystem::path& path)
    {
        OTIO_NS::ErrorStatus errorStatus;
        auto timeline = OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>(
            dynamic_cast<OTIO_NS::Timeline*>(OTIO_NS::Timeline::from_json_file(path.string(), &errorStatus)));
        if (!timeline)
        {
            throw std::runtime_error(errorStatus.full_description);
        }

        auto files = _files->get();
        files.push_back({ path, timeline });
        _files->setIfChanged(files);
        _current->setIfChanged(files.size() - 1);
    }

    void FilesModel::close()
    {
        auto files = _files->get();
        int current = _current->get();
        if (current >= 0 && current < files.size())
        {
            files.erase(files.begin() + current);
            _files->setIfChanged(files);
            current = std::min(current, static_cast<int>(files.size()) - 1);
            _current->setAlways(current);
        }
    }

    std::shared_ptr<dtk::IObservableList<File> > FilesModel::observeFiles() const
    {
        return _files;
    }

    std::shared_ptr<dtk::IObservableValue<int> > FilesModel::observeCurrent() const
    {
        return _current;
    }
}
