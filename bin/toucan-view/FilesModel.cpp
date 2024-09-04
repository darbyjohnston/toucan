// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "FilesModel.h"

#include <dtk/core/Math.h>

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
        _currentIndex = dtk::ObservableValue<int>::create(-1);
        _currentFile = dtk::ObservableValue<File>::create();
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
        const int index = files.size() - 1;
        _currentIndex->setIfChanged(index);
        _currentFile->setIfChanged(files[index]);
    }

    void FilesModel::close()
    {
        auto files = _files->get();
        int current = _currentIndex->get();
        if (current >= 0 && current < files.size())
        {
            files.erase(files.begin() + current);
            _files->setIfChanged(files);
            current = std::min(current, static_cast<int>(files.size()) - 1);
            _currentIndex->setAlways(current);
            _currentFile->setAlways(
                (current >= 0 && current < files.size()) ?
                files[current] :
                File());
        }
    }

    std::shared_ptr<dtk::IObservableList<File> > FilesModel::observeFiles() const
    {
        return _files;
    }

    std::shared_ptr<dtk::IObservableValue<int> > FilesModel::observeCurrentIndex() const
    {
        return _currentIndex;
    }

    std::shared_ptr<dtk::IObservableValue<File> > FilesModel::observeCurrentFile() const
    {
        return _currentFile;
    }

    void FilesModel::setCurrentIndex(int value)
    {
        const auto& files = _files->get();
        const int index = dtk::clamp(value, 0, static_cast<int>(files.size()) - 1);
        _currentIndex->setIfChanged(index);
        _currentFile->setIfChanged(files[index]);
    }

    void FilesModel::next()
    {
        const auto& files = _files->get();
        if (!files.empty())
        {
            int index = _currentIndex->get() + 1;
            if (index >= files.size())
            {
                index = 0;
            }
            setCurrentIndex(index);
        }
    }

    void FilesModel::prev()
    {
        const auto& files = _files->get();
        if (!files.empty())
        {
            int index = _currentIndex->get() - 1;
            if (index < 0)
            {
                index = static_cast<int>(files.size()) - 1;
            }
            setCurrentIndex(index);
        }
    }
}
