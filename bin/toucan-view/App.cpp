// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "App.h"

#include "FilesModel.h"
#include "PlaybackModel.h"

#include <dtkUI/FileBrowser.h>
#include <dtkUI/MessageDialog.h>

#include <dtkBaseApp/CmdLine.h>

using namespace dtk;
using namespace dtk::core;
using namespace dtk::ui;

namespace toucan
{
    void App::_init(
        const std::shared_ptr<Context>& context,
        std::vector<std::string>& argv)
    {
        ui::App::_init(
            context,
            argv,
            "toucan-view",
            "Toucan viewer",
            {
                app::CmdLineValueArg<std::filesystem::path>::create(
                    _path,
                    "input",
                    "Input timeline.",
                    true)
            });

        context->getSystem<FileBrowserSystem>()->setNativeFileDialog(false);

        _messageLog = std::make_shared<MessageLog>();

        _filesModel = std::make_shared<FilesModel>(context);

        _playbackModel = std::make_shared<PlaybackModel>(context);

        _currentImage = ObservableValue<std::shared_ptr<Image> >::create();

        _filesObserver = ListObserver<File>::create(
            _filesModel->observeFiles(),
            [this](const std::vector<File>& value)
            {
                _files = value;
            });

        _currentFileObserver = ValueObserver<int>::create(
            _filesModel->observeCurrent(),
            [this](int value)
            {
                _currentFile = value;
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
                        _path.parent_path(),
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

        _currentTimeObserver = ValueObserver<OTIO_NS::RationalTime>::create(
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
            Size2I(1920, 1080));
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
        const std::shared_ptr<Context>& context,
        std::vector<std::string>& argv)
    {
        auto out = std::shared_ptr<App>(new App);
        out->_init(context, argv);
        return out;
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
            _context->getSystem<MessageDialogSystem>()->message("ERROR", e.what(), _window);
        }
    }

    const std::shared_ptr<PlaybackModel>& App::getPlaybackModel() const
    {
        return _playbackModel;
    }

    std::shared_ptr<dtk::core::IObservableValue<std::shared_ptr<Image> > > App::observeCurrentImage() const
    {
        return _currentImage;
    }

    void App::_render()
    {
        std::shared_ptr<Image> image;
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
            ImageType imageType = ImageType::None;
            if (OIIO::TypeDesc::UINT8 == spec.format)
            {
                switch (spec.nchannels)
                {
                case 1: imageType = ImageType::L_U8; break;
                case 2: imageType = ImageType::LA_U8; break;
                case 3: imageType = ImageType::RGB_U8; break;
                case 4: imageType = ImageType::RGBA_U8; break;
                default: break;
                }
            }
            else if (OIIO::TypeDesc::UINT16 == spec.format)
            {
                switch (spec.nchannels)
                {
                case 1: imageType = ImageType::L_U16; break;
                case 2: imageType = ImageType::LA_U16; break;
                case 3: imageType = ImageType::RGB_U16; break;
                case 4: imageType = ImageType::RGBA_U16; break;
                default: break;
                }
            }
            else if (OIIO::TypeDesc::HALF == spec.format)
            {
                switch (spec.nchannels)
                {
                case 1: imageType = ImageType::L_F16; break;
                case 2: imageType = ImageType::LA_F16; break;
                case 3: imageType = ImageType::RGB_F16; break;
                case 4: imageType = ImageType::RGBA_F16; break;
                default: break;
                }
            }
            else if (OIIO::TypeDesc::FLOAT == spec.format)
            {
                switch (spec.nchannels)
                {
                case 1: imageType = ImageType::L_F32; break;
                case 2: imageType = ImageType::LA_F32; break;
                case 3: imageType = ImageType::RGB_F32; break;
                case 4: imageType = ImageType::RGBA_F32; break;
                default: break;
                }
            }
            ImageInfo info(spec.width, spec.height, imageType);
            info.layout.mirror.y = true;
            image = Image::create(info, reinterpret_cast<uint8_t*>(_imageBuf.localpixels()));
        }
        _currentImage->setIfChanged(image);
    }
}
