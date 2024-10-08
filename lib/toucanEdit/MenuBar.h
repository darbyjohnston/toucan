// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucanEdit/PlaybackModel.h>
#include <toucanEdit/TimeUnitsModel.h>

#include <dtk/ui/MenuBar.h>
#include <dtk/core/ObservableList.h>

namespace toucan
{
    class App;
    class Document;
    class DocumentsModel;
    class Window;

    class MenuBar : public dtk::MenuBar
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Window>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~MenuBar();

        static std::shared_ptr<MenuBar> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Window>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        const std::map<std::string, std::shared_ptr<dtk::Action> >& getActions() const;

    private:
        void _fileMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _editMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _timeMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _playbackMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _viewMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&);
        void _windowMenuInit(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Window>&);

        void _fileMenuUpdate();
        void _editMenuUpdate();
        void _timeMenuUpdate();
        void _playbackMenuUpdate();
        void _viewMenuUpdate();
        void _windowMenuUpdate();

        std::weak_ptr<App> _app;
        std::shared_ptr<DocumentsModel> _documentsModel;
        std::shared_ptr<Document> _document;

        std::map<std::string, std::shared_ptr<dtk::Menu> > _menus;
        std::map<std::string, std::shared_ptr<dtk::Action> > _actions;
        std::vector<std::shared_ptr<dtk::Action> > _filesActions;

        std::shared_ptr<dtk::ListObserver<std::shared_ptr<Document> > > _documentsObserver;
        std::shared_ptr<dtk::ValueObserver<std::shared_ptr<Document> > > _documentObserver;
        std::shared_ptr<dtk::ValueObserver<int> > _documentIndexObserver;
        std::shared_ptr<dtk::ValueObserver<Playback> > _playbackObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _frameViewObserver;
        std::shared_ptr<dtk::ValueObserver<bool> > _fullScreenObserver;
        std::shared_ptr<dtk::ValueObserver<float> > _displayScaleObserver;
        std::shared_ptr<dtk::ValueObserver<TimeUnits> > _timeUnitsObserver;
    };
}

