// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "PlaybackMenu.h"

#include "App.h"
#include "FilesModel.h"

#include <dtk/ui/Action.h>

namespace toucan
{
    void PlaybackMenu::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::Menu::_init(context, parent);

        _actions["Playback/Stop"] = std::make_shared<dtk::Action>(
            "Stop",
            "PlaybackStop",
            dtk::Key::K,
            0,
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->setPlayback(Playback::Stop);
                }
            });
        addItem(_actions["Playback/Stop"]);

        _actions["Playback/Forward"] = std::make_shared<dtk::Action>(
            "Forward",
            "PlaybackForward",
            dtk::Key::L,
            0,
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->setPlayback(Playback::Forward);
                }
            });
        addItem(_actions["Playback/Forward"]);

        _actions["Playback/Reverse"] = std::make_shared<dtk::Action>(
            "Reverse",
            "PlaybackReverse",
            dtk::Key::J,
            0,
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->setPlayback(Playback::Reverse);
                }
            });
        addItem(_actions["Playback/Reverse"]);

        addDivider();

        _actions["Playback/Toggle"] = std::make_shared<dtk::Action>(
            "Toggle Playback",
            dtk::Key::Space,
            0,
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->togglePlayback();
                }
            });
        addItem(_actions["Playback/Toggle"]);

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                _menuUpdate();
            });
    }

    PlaybackMenu::~PlaybackMenu()
    {}

    std::shared_ptr<PlaybackMenu> PlaybackMenu::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<PlaybackMenu>(new PlaybackMenu);
        out->_init(context, app, parent);
        return out;
    }

    const std::map<std::string, std::shared_ptr<dtk::Action> >& PlaybackMenu::getActions() const
    {
        return _actions;
    }

    void PlaybackMenu::_menuUpdate()
    {
        const bool file = _file.get();
        if (file)
        {
            _playbackObserver = dtk::ValueObserver<Playback>::create(
                _file->getPlaybackModel()->observePlayback(),
                [this](Playback value)
                {
                    setItemChecked(_actions["Playback/Stop"], Playback::Stop == value);
                    setItemChecked(_actions["Playback/Forward"], Playback::Forward == value);
                    setItemChecked(_actions["Playback/Reverse"], Playback::Reverse == value);
                });
        }
        else
        {
            _playbackObserver.reset();
        }

        setItemEnabled(_actions["Playback/Stop"], file);
        setItemEnabled(_actions["Playback/Forward"], file);
        setItemEnabled(_actions["Playback/Reverse"], file);
        setItemEnabled(_actions["Playback/Toggle"], file);
    }
}
