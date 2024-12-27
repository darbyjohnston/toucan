// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FilesModel.h"

#include "PlaybackModel.h"

#include <dtk/core/Error.h>
#include <dtk/core/Math.h>
#include <dtk/core/String.h>

#include <nlohmann/json.hpp>

#include <sstream>

namespace toucan
{
    DTK_ENUM_IMPL(
        CompareMode,
        "A",
        "B",
        "Split",
        "Overlay",
        "Horizontal",
        "Vertical");

    bool CompareOptions::operator == (const CompareOptions& other) const
    {
        return
            mode == other.mode &&
            matchStartTime == other.matchStartTime &&
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
        _settings(context->getSystem<dtk::Settings>()),
        _host(host)
    {
        CompareOptions compareOptions;
        try
        {
            const auto json = std::any_cast<nlohmann::json>(_settings->get("FilesModel"));
            auto i = json.find("CompareMode");
            if (i != json.end() && i->is_string())
            {
                std::stringstream ss(i->get<std::string>());
                ss >> compareOptions.mode;
            }
            i = json.find("MatchStartTime");
            if (i != json.end() && i->is_boolean())
            {
                compareOptions.matchStartTime = i->get<bool>();
            }
            i = json.find("FitSize");
            if (i != json.end() && i->is_boolean())
            {
                compareOptions.fitSize = i->get<bool>();
            }
        }
        catch (const std::exception&)
        {}

        _files = dtk::ObservableList< std::shared_ptr<File> >::create();
        _add = dtk::ObservableValue<int>::create(-1);
        _remove = dtk::ObservableValue<int>::create(-1);
        _current = dtk::ObservableValue< std::shared_ptr<File> >::create(nullptr);
        _currentIndex = dtk::ObservableValue<int>::create(-1);
        _bFile = dtk::ObservableValue<std::shared_ptr<File> >::create();
        _bIndex = dtk::ObservableValue<int>::create(-1);
        _compareOptions = dtk::ObservableValue<CompareOptions>::create(compareOptions);
        _recentFilesModel = dtk::RecentFilesModel::create(context);
    }

    FilesModel::~FilesModel()
    {
        nlohmann::json json;
        {
            std::stringstream ss;
            ss << _compareOptions->get().mode;
            json["CompareMode"] = ss.str();
        }
        json["MatchStartTime"] = _compareOptions->get().matchStartTime;
        json["FitSize"] = _compareOptions->get().fitSize;
        _settings->set("FilesModel", json);
    }

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
        const auto& files = _files->get();
        const int index = dtk::clamp(value, -1, static_cast<int>(files.size()) - 1);
        if (_bIndex->setIfChanged(index))
        {
            auto file = _current->get();
            auto bFile = _getBFile();
            if (file && bFile)
            {
                if (bFile != file)
                {
                    bFile->getPlaybackModel()->setPlayback(Playback::Stop);
                }
                const OTIO_NS::RationalTime time =
                    file->getPlaybackModel()->getCurrentTime() +
                    _getBTimeOffset(file, bFile);
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
        if (_compareOptions->setIfChanged(value))
        {
            auto file = _current->get();
            auto bFile = _bFile->get();
            if (file && bFile)
            {
                bFile->getPlaybackModel()->setCurrentTime(
                    file->getPlaybackModel()->getCurrentTime() +
                    _getBTimeOffset(file, bFile));
            }
        }
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

    OTIO_NS::RationalTime FilesModel::_getBTimeOffset(
        const std::shared_ptr<File>& file,
        const std::shared_ptr<File>& bFile) const
    {
        OTIO_NS::RationalTime out(0.0, 1.0);
        if (_compareOptions->get().matchStartTime)
        {
            out = bFile->getPlaybackModel()->getTimeRange().start_time() -
                file->getPlaybackModel()->getTimeRange().start_time();
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
                    auto file = _current->get();
                    auto bFile = _bFile->get();
                    if (file && bFile)
                    {
                        bFile->getPlaybackModel()->setCurrentTime(
                            value +
                            _getBTimeOffset(file, bFile));
                    }
                });
        }
        else
        {
            _currentTimeObserver.reset();
        }
    }
}
