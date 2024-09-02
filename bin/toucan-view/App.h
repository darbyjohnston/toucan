// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "FilesModel.h"
#include "PlaybackModel.h"
#include "Type.h"
#include "Window.h"

#include <toucan/ImageEffectHost.h>
#include <toucan/ImageGraph.h>

#include <dtkUIApp/App.h>

#include <dtkCore/Image.h>

namespace toucan
{
    class App : public dtk::ui::App
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            std::vector<std::string>&);

    public:
        virtual ~App();

        static std::shared_ptr<App> create(
            const std::shared_ptr<dtk::core::Context>&,
            std::vector<std::string>&);

        const std::shared_ptr<FilesModel>& getFilesModel() const;

        void open(const std::filesystem::path&);

        const std::shared_ptr<PlaybackModel>& getPlaybackModel() const;

        std::shared_ptr<dtk::core::IObservableValue<std::shared_ptr<dtk::core::Image> > > observeCurrentImage() const;

    private:
        void _render();

        std::shared_ptr<MessageLog> _messageLog;
        std::filesystem::path _path;
        std::shared_ptr<FilesModel> _filesModel;
        std::shared_ptr<PlaybackModel> _playbackModel;
        std::shared_ptr<dtk::core::ObservableValue<std::shared_ptr<dtk::core::Image> > > _currentImage;
        std::shared_ptr<dtk::core::ListObserver<File> > _filesObserver;
        std::vector<File> _files;
        std::shared_ptr<dtk::core::ValueObserver<int> > _currentFileObserver;
        int _currentFile = -1;
        std::shared_ptr<dtk::core::ValueObserver<OTIO_NS::RationalTime> > _currentTimeObserver;
        OTIO_NS::RationalTime _currentTime;
        OIIO::ImageBuf _imageBuf;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<ImageGraph> _graph;
        std::shared_ptr<Window> _window;
    };
}

