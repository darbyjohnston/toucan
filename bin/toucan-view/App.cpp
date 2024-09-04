// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "App.h"

#include "FilesModel.h"
#include "PlaybackModel.h"

#include <dtk/ui/FileBrowser.h>
#include <dtk/ui/MessageDialog.h>

#include <dtk/core/CmdLine.h>

namespace toucan
{
    void App::_init(
        const std::shared_ptr<dtk::Context>& context,
        std::vector<std::string>& argv)
    {
        dtk::App::_init(
            context,
            argv,
            "toucan-view",
            "Toucan viewer",
            {
                dtk::CmdLineValueArg<std::filesystem::path>::create(
                    _path,
                    "input",
                    "Input timeline.",
                    true)
            });

        context->getSystem<dtk::FileBrowserSystem>()->setNativeFileDialog(false);

        _messageLog = std::make_shared<MessageLog>();
        _timeUnitsModel = std::make_shared<TimeUnitsModel>();
        _filesModel = std::make_shared<FilesModel>(context);
        _playbackModel = std::make_shared<PlaybackModel>(context);
        _viewModel = std::make_shared<ViewModel>();
        _currentImage = dtk::ObservableValue<std::shared_ptr<dtk::Image> >::create();

        _filesObserver = dtk::ListObserver<File>::create(
            _filesModel->observeFiles(),
            [this](const std::vector<File>& value)
            {
                _files = value;
            });

        _currentIndexObserver = dtk::ValueObserver<int>::create(
            _filesModel->observeCurrentIndex(),
            [this](int value)
            {
                if (value >= 0 && value < static_cast<int>(_files.size()))
                {
                    const auto& file = _files[value];
                    const auto& globalStartTime = file.timeline->global_start_time();
                    const OTIO_NS::RationalTime& duration = file.timeline->duration();
                    const OTIO_NS::RationalTime startTime = globalStartTime.has_value() ?
                        globalStartTime.value() :
                        OTIO_NS::RationalTime(0.0, duration.rate());
                    _playbackModel->setTimeRange(OTIO_NS::TimeRange(startTime, duration));
                    _playbackModel->setCurrentTime(startTime);

                    ImageGraphOptions graphOptions;
                    _graph = std::make_shared<ImageGraph>(
                        file.path.parent_path(),
                        file.timeline,
                        graphOptions);
                    _render();
                }
                else
                {
                    _playbackModel->setTimeRange(OTIO_NS::TimeRange());
                    _playbackModel->setCurrentTime(OTIO_NS::RationalTime(-1.0, -1.0));
                    _currentImage->setIfChanged(nullptr);
                    _graph.reset();
                    _imageBuf = OIIO::ImageBuf();
                }
            });

        _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
            _playbackModel->observeCurrentTime(),
            [this](const OTIO_NS::RationalTime& value)
            {
                _currentTime = value;
                _render();
            });

        std::vector<std::filesystem::path> searchPath;
        const std::filesystem::path parentPath = std::filesystem::path(argv[0]).parent_path();
        searchPath.push_back(parentPath);
#if defined(_WINDOWS)
        searchPath.push_back(parentPath / ".." / ".." / "..");
#else // _WINDOWS
        searchPath.push_back(parentPath / ".." / "..");
#endif // _WINDOWS
        ImageEffectHostOptions hostOptions;
        //hostOptions.log = _messageLog;
        _host = std::make_shared<ImageEffectHost>(searchPath, hostOptions);

        _window = Window::create(
            context,
            std::dynamic_pointer_cast<App>(shared_from_this()),
            "toucan-view",
            dtk::Size2I(1920, 1080));
        addWindow(_window);

        if (!_path.empty())
        {
            open(_path);
        }

        _window->show();
    }

    App::~App()
    {}

    std::shared_ptr<App> App::create(
        const std::shared_ptr<dtk::Context>& context,
        std::vector<std::string>& argv)
    {
        auto out = std::shared_ptr<App>(new App);
        out->_init(context, argv);
        return out;
    }

    const std::shared_ptr<TimeUnitsModel>& App::getTimeUnitsModel() const
    {
        return _timeUnitsModel;
    }

    const std::shared_ptr<FilesModel>& App::getFilesModel() const
    {
        return _filesModel;
    }

    void App::open(const std::filesystem::path& path)
    {
        _playbackModel->setPlayback(Playback::Stop);
        try
        {
            _filesModel->open(path);
        }
        catch (const std::exception& e)
        {
            _context->getSystem<dtk::MessageDialogSystem>()->message("ERROR", e.what(), _window);
        }
    }

    const std::shared_ptr<PlaybackModel>& App::getPlaybackModel() const
    {
        return _playbackModel;
    }

    const std::shared_ptr<ViewModel>& App::getViewModel() const
    {
        return _viewModel;
    }

    std::shared_ptr<dtk::IObservableValue<std::shared_ptr<dtk::Image> > > App::observeCurrentImage() const
    {
        return _currentImage;
    }

    void App::_render()
    {
        std::shared_ptr<dtk::Image> image;
        if (_graph)
        {
            auto node = _graph->exec(_host, _currentTime);
            /*for (auto i : node->graph(currentTime, "foo"))
            {
                std::cout << i << std::endl;
            }
            std::cout << std::endl;*/

            const OTIO_NS::TimeRange& timeRange = _playbackModel->getTimeRange();
            _imageBuf = node->exec(_currentTime - timeRange.start_time());
            const auto& spec = _imageBuf.spec();
            dtk::ImageType imageType = dtk::ImageType::None;
            if (OIIO::TypeDesc::UINT8 == spec.format)
            {
                switch (spec.nchannels)
                {
                case 1: imageType = dtk::ImageType::L_U8; break;
                case 2: imageType = dtk::ImageType::LA_U8; break;
                case 3: imageType = dtk::ImageType::RGB_U8; break;
                case 4: imageType = dtk::ImageType::RGBA_U8; break;
                default: break;
                }
            }
            else if (OIIO::TypeDesc::UINT16 == spec.format)
            {
                switch (spec.nchannels)
                {
                case 1: imageType = dtk::ImageType::L_U16; break;
                case 2: imageType = dtk::ImageType::LA_U16; break;
                case 3: imageType = dtk::ImageType::RGB_U16; break;
                case 4: imageType = dtk::ImageType::RGBA_U16; break;
                default: break;
                }
            }
            else if (OIIO::TypeDesc::HALF == spec.format)
            {
                switch (spec.nchannels)
                {
                case 1: imageType = dtk::ImageType::L_F16; break;
                case 2: imageType = dtk::ImageType::LA_F16; break;
                case 3: imageType = dtk::ImageType::RGB_F16; break;
                case 4: imageType = dtk::ImageType::RGBA_F16; break;
                default: break;
                }
            }
            else if (OIIO::TypeDesc::FLOAT == spec.format)
            {
                switch (spec.nchannels)
                {
                case 1: imageType = dtk::ImageType::L_F32; break;
                case 2: imageType = dtk::ImageType::LA_F32; break;
                case 3: imageType = dtk::ImageType::RGB_F32; break;
                case 4: imageType = dtk::ImageType::RGBA_F32; break;
                default: break;
                }
            }
            dtk::ImageInfo info(spec.width, spec.height, imageType);
            info.layout.mirror.y = true;
            image = dtk::Image::create(info, reinterpret_cast<uint8_t*>(_imageBuf.localpixels()));
        }
        _currentImage->setIfChanged(image);
    }
}
