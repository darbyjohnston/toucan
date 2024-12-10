// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FilesModel.h"

#include <toucan/ImageEffectHost.h>

#include <dtk/core/Math.h>

namespace toucan
{
    FilesModel::FilesModel(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<ImageEffectHost>& host) :
        _context(context),
        _host(host)
    {
        _files = dtk::ObservableList< std::shared_ptr<File> >::create();
        _add = dtk::ObservableValue<int>::create(-1);
        _remove = dtk::ObservableValue<int>::create(-1);
        _current = dtk::ObservableValue< std::shared_ptr<File> >::create(nullptr);
        _currentIndex = dtk::ObservableValue<int>::create(-1);
        _recentFilesModel = dtk::RecentFilesModel::create(context);
    }

    FilesModel::~FilesModel()
    {}

    void FilesModel::open(const std::filesystem::path& path)
    {
        if (auto context = _context.lock())
        {
            auto file = std::make_shared<File>(context, _host, path);
            auto files = _files->get();
            files.push_back(file);
            _files->setIfChanged(files);
            const int index = files.size() - 1;
            _add->setAlways(index);
            _current->setIfChanged(files[index]);
            _currentIndex->setIfChanged(index);
            _recentFilesModel->addRecent(path);
        }
    }

    void FilesModel::close()
    {
        close(_currentIndex->get());
    }

    void FilesModel::close(int index)
    {
        auto files = _files->get();
        if (index >= 0 && index < files.size())
        {
            auto file = *(files.begin() + index);
            files.erase(files.begin() + index);
            _remove->setAlways(index);
            _files->setIfChanged(files);
            int current = std::min(_currentIndex->get(), static_cast<int>(files.size()) - 1);
            _current->setAlways(
                (current >= 0 && current < files.size()) ?
                files[current] :
                nullptr);
            _currentIndex->setAlways(current);
        }
    }

    void FilesModel::closeAll()
    {
        auto files = _files->get();
        for (size_t i = 0; i < files.size(); ++i)
        {
            _remove->setAlways(i);
        }
        _files->setIfChanged({});
        _current->setIfChanged(nullptr);
        _currentIndex->setIfChanged(-1);
    }

    std::shared_ptr<dtk::IObservableList<std::shared_ptr<File> > > FilesModel::observeFiles() const
    {
        return _files;
    }

    std::shared_ptr<dtk::IObservableValue<int> > FilesModel::observeAdd() const
    {
        return _add;
    }

    std::shared_ptr<dtk::IObservableValue<int> > FilesModel::observeRemove() const
    {
        return _remove;
    }

    std::shared_ptr<dtk::IObservableValue<std::shared_ptr<File> > > FilesModel::observeCurrent() const
    {
        return _current;
    }

    std::shared_ptr<dtk::IObservableValue<int> > FilesModel::observeCurrentIndex() const
    {
        return _currentIndex;
    }

    void FilesModel::setCurrentIndex(int value)
    {
        const auto& files = _files->get();
        const int index = dtk::clamp(value, 0, static_cast<int>(files.size()) - 1);
        _current->setIfChanged(files[index]);
        _currentIndex->setIfChanged(index);
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

    const std::shared_ptr<dtk::RecentFilesModel>& FilesModel::getRecentFilesModel() const
    {
        return _recentFilesModel;
    }
}
