// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "FilesModel.h"
#include "PlaybackModel.h"
#include "TimeUnitsModel.h"
#include "ViewModel.h"
#include "Window.h"

#include <toucan/ImageEffectHost.h>
#include <toucan/ImageGraph.h>

#include <dtk/ui/App.h>

#include <dtk/core/Image.h>
#include <dtk/core/ObservableValue.h>

namespace toucan
{
    class App : public dtk::App
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            std::vector<std::string>&);

    public:
        virtual ~App();

        static std::shared_ptr<App> create(
            const std::shared_ptr<dtk::Context>&,
            std::vector<std::string>&);

        const std::shared_ptr<TimeUnitsModel>& getTimeUnitsModel() const;

        const std::shared_ptr<FilesModel>& getFilesModel() const;

        void open(const std::filesystem::path&);

        const std::shared_ptr<PlaybackModel>& getPlaybackModel() const;

        const std::shared_ptr<ViewModel>& getViewModel() const;

        std::shared_ptr<dtk::IObservableValue<std::shared_ptr<dtk::Image> > > observeCurrentImage() const;

    private:
        void _render();

        std::shared_ptr<MessageLog> _messageLog;
        std::filesystem::path _path;
        std::shared_ptr<TimeUnitsModel> _timeUnitsModel;
        std::shared_ptr<FilesModel> _filesModel;
        std::shared_ptr<PlaybackModel> _playbackModel;
        std::shared_ptr<ViewModel> _viewModel;
        std::shared_ptr<dtk::ObservableValue<std::shared_ptr<dtk::Image> > > _currentImage;
        std::shared_ptr<dtk::ListObserver<File> > _filesObserver;
        std::vector<File> _files;
        std::shared_ptr<dtk::ValueObserver<int> > _currentIndexObserver;
        std::shared_ptr<dtk::ValueObserver<OTIO_NS::RationalTime> > _currentTimeObserver;
        OTIO_NS::RationalTime _currentTime;
        OIIO::ImageBuf _imageBuf;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<ImageGraph> _graph;
        std::shared_ptr<Window> _window;
    };
}

