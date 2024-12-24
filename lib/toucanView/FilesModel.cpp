// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FilesModel.h"

#include "PlaybackModel.h"

#include <dtk/core/Error.h>
#include <dtk/core/Math.h>
#include <dtk/core/String.h>

namespace toucan
{
    DTK_ENUM_IMPL(
        CompareMode,
        "A",
        "B",
        "Split",
        "Horizontal",
        "Vertical");

    DTK_ENUM_IMPL(
        CompareTime,
        "Relative",
        "Absolute");

    bool CompareOptions::operator == (const CompareOptions& other) const
    {
        return
            mode == other.mode &&
            time == other.time &&
            fitSize == other.fitSize;;
    }

    bool CompareOptions::operator != (const CompareOptions& other) const
    {
        return !(*this == other);
    }

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
        _bFile = dtk::ObservableValue<std::shared_ptr<File> >::create();
        _bIndex = dtk::ObservableValue<int>::create(-1);
        _compareOptions = dtk::ObservableValue<CompareOptions>::create();
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

            _fileUpdate();
        }
    }

    void FilesModel::close()
    {
        close(_currentIndex->get());
    }

    void FilesModel::close(int index)
    {
        auto files = _files->get();
        std::shared_ptr<File> file;
        if (index >= 0 && index < files.size())
        {
            file = *(files.begin() + index);
            files.erase(files.begin() + index);
            _remove->setAlways(index);
            _files->setIfChanged(files);

            int current = std::min(_currentIndex->get(), static_cast<int>(files.size()) - 1);
            _current->setAlways(
                (current >= 0 && current < files.size()) ?
                files[current] :
                nullptr);
            _currentIndex->setAlways(current);

            if (_bIndex->get() == index)
            {
                _bIndex->setIfChanged(-1);
                _bFile->setIfChanged(_getBFile());
            }
        }
        _fileUpdate();
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

        _bFile->setIfChanged(nullptr);
        _bIndex->setIfChanged(-1);

        _currentTimeObserver.reset();
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
        _current->setIfChanged(index >= 0 ? files[index] : nullptr);
        _currentIndex->setIfChanged(index);
        _fileUpdate();
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

    const std::shared_ptr<File>& FilesModel::getBFile() const
    {
        return _bFile->get();
    }

    std::shared_ptr<dtk::IObservableValue<std::shared_ptr<File> > > FilesModel::observeBFile() const
    {
        return _bFile;
    }

    int FilesModel::getBIndex() const
    {
        return _bIndex->get();
    }

    std::shared_ptr<dtk::IObservableValue<int> > FilesModel::observeBIndex() const
    {
        return _bIndex;
    }

    void FilesModel::setBIndex(int value)
    {
        if (_bIndex->setIfChanged(value))
        {
            auto file = _current->get();
            OTIO_NS::RationalTime time;
            if (file)
            {
                time = file->getPlaybackModel()->getCurrentTime();
            }
            auto bFile = _getBFile();
            if (bFile)
            {
                if (bFile != file)
                {
                    bFile->getPlaybackModel()->setPlayback(Playback::Stop);
                }
                bFile->getPlaybackModel()->setCurrentTime(time);
            }
            _bFile->setIfChanged(bFile);
        }
    }

    const CompareOptions& FilesModel::getCompareOptions() const
    {
        return _compareOptions->get();
    }

    std::shared_ptr<dtk::IObservableValue<CompareOptions> > FilesModel::observeCompareOptions() const
    {
        return _compareOptions;
    }

    void FilesModel::setCompareOptions(const CompareOptions& value)
    {
        _compareOptions->setIfChanged(value);
    }

    const std::shared_ptr<dtk::RecentFilesModel>& FilesModel::getRecentFilesModel() const
    {
        return _recentFilesModel;
    }

    std::shared_ptr<File> FilesModel::_getBFile() const
    {
        std::shared_ptr<File> out;
        const std::vector<std::shared_ptr<File> > files = _files->get();
        const int bIndex = _bIndex->get();
        if (bIndex >= 0 && bIndex < files.size())
        {
            out = files[bIndex];
        }
        return out;
    }

    void FilesModel::_fileUpdate()
    {
        if (auto file = _current->get())
        {
            _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
                file->getPlaybackModel()->observeCurrentTime(),
                [this](const OTIO_NS::RationalTime& value)
                {
                    if (auto file = _bFile->get())
                    {
                        file->getPlaybackModel()->setCurrentTime(value);
                    }
                });
        }
        else
        {
            _currentTimeObserver.reset();
        }
    }
}
