// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "App.h"

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

        _timeline = ObservableValue<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> >::create();
        _timeRange = ObservableValue<OTIO_NS::TimeRange>::create();
        _playback = ObservableValue<Playback>::create(Playback::Stop);
        _currentTime = ObservableValue<OTIO_NS::RationalTime>::create(OTIO_NS::RationalTime(-1.0, -1.0));
        _currentImage = ObservableValue<std::shared_ptr<Image> >::create();

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

    void App::open(const std::filesystem::path& path)
    {
        setPlayback(Playback::Stop);

        try
        {
            _path = path;

            OTIO_NS::ErrorStatus errorStatus;
            auto timeline = OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>(
                dynamic_cast<OTIO_NS::Timeline*>(OTIO_NS::Timeline::from_json_file(_path.string(), &errorStatus)));
            if (!timeline)
            {
                std::stringstream ss;
                ss << _path.string() << ": " << errorStatus.full_description << std::endl;
                throw std::runtime_error(ss.str());
            }

            _timeline->setIfChanged(timeline);
            const auto& globalStartTime = timeline->global_start_time();
            const OTIO_NS::RationalTime& duration = timeline->duration();
            const OTIO_NS::RationalTime startTime = globalStartTime.has_value() ?
                globalStartTime.value() :
                OTIO_NS::RationalTime(0.0, duration.rate());
            const OTIO_NS::TimeRange timeRange(startTime, duration);
            _timeRange->setIfChanged(timeRange);
            _currentTime->setIfChanged(startTime);

            ImageGraphOptions graphOptions;
            _graph = std::make_shared<ImageGraph>(_path.parent_path(), timeline, graphOptions);
        }
        catch (const std::exception& e)
        {
            _context->getSystem<MessageDialogSystem>()->message("ERROR", e.what(), _window);
        }

        _render();
    }

    void App::close()
    {
        if (!_path.empty())
        {
            _timeline->setIfChanged(nullptr);
            _timeRange->setIfChanged(OTIO_NS::TimeRange());
            _currentTime->setIfChanged(OTIO_NS::RationalTime());
            _currentImage->setIfChanged(nullptr);
            _imageBuf = OIIO::ImageBuf();
        }
    }

    std::shared_ptr<IObservableValue<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> > > App::observeTimeline() const
    {
        return _timeline;
    }

    std::shared_ptr<IObservableValue<OTIO_NS::TimeRange> > App::observeTimeRange() const
    {
        return _timeRange;
    }

    std::shared_ptr<IObservableValue<Playback> > App::observePlayback() const
    {
        return _playback;
    }

    void App::setPlayback(Playback value)
    {
        if (_playback->setIfChanged(value))
        {
            _playbackUpdate();
        }
    }

    std::shared_ptr<IObservableValue<OTIO_NS::RationalTime> > App::observeCurrentTime() const
    {
        return _currentTime;
    }

    void App::setCurrentTime(const OTIO_NS::RationalTime& value)
    {
        const OTIO_NS::RationalTime tmp = _timeRange->get().clamped(value);
        if (_currentTime->setIfChanged(tmp))
        {
            _render();
        }
    }

    std::shared_ptr<dtk::core::IObservableValue<std::shared_ptr<Image> > > App::observeCurrentImage() const
    {
        return _currentImage;
    }

    void App::_playbackUpdate()
    {
        switch (_playback->get())
        {
        case Playback::Stop:
            _timer.reset();
            break;
        case Playback::Forward:
        case Playback::Reverse:
            _timer = Timer::create(_context);
            _timer->setRepeating(true);
            _timer->start(
                std::chrono::microseconds(static_cast<int>(1000 / _currentTime->get().rate())),
                [this]
                {
                    _timerUpdate();
                });
            break;
        default: break;
        }
    }

    void App::_timerUpdate()
    {
        switch (_playback->get())
        {
        case Playback::Forward:
        {
            auto time = _currentTime->get() + OTIO_NS::RationalTime(1.0, _currentTime->get().rate());
            if (time > _timeRange->get().end_time_inclusive())
            {
                time = _timeRange->get().start_time();
            }
            setCurrentTime(time);
            break;
        }
        case Playback::Reverse:
        {
            auto time = _currentTime->get() - OTIO_NS::RationalTime(1.0, _currentTime->get().rate());
            if (time < _timeRange->get().start_time())
            {
                time = _timeRange->get().end_time_inclusive();
            }
            setCurrentTime(time);
            break;
        }
        default: break;
        }
    }

    void App::_render()
    {
        std::shared_ptr<Image> image;
        if (_timeline->get() && _graph)
        {
            const OTIO_NS::RationalTime& currentTime = _currentTime->get();
            auto node = _graph->exec(_host, currentTime);
            /*for (auto i : node->graph(currentTime, "foo"))
            {
                std::cout << i << std::endl;
            }
            std::cout << std::endl;*/

            _imageBuf = node->exec(currentTime - _timeRange->get().start_time());
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
