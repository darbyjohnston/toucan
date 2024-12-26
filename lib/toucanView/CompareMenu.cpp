// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "CompareMenu.h"

#include "App.h"

#include <dtk/ui/Action.h>

namespace toucan
{
    void CompareMenu::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::Menu::_init(context, parent);

        _filesModel = app->getFilesModel();

        _menus["BFile"] = addSubMenu("B File");

        addDivider();

        _actions["Compare/A"] = std::make_shared<dtk::Action>(
            "A",
            dtk::Key::A,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this](bool)
            {
                CompareOptions options = _filesModel->getCompareOptions();
                options.mode = CompareMode::A;
                _filesModel->setCompareOptions(options);
            });
        addItem(_actions["Compare/A"]);

        _actions["Compare/B"] = std::make_shared<dtk::Action>(
            "B",
            dtk::Key::B,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this](bool)
            {
                CompareOptions options = _filesModel->getCompareOptions();
                options.mode = CompareMode::B;
                _filesModel->setCompareOptions(options);
            });
        addItem(_actions["Compare/B"]);

        _actions["Compare/Split"] = std::make_shared<dtk::Action>(
            "Split",
            dtk::Key::S,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this](bool)
            {
                CompareOptions options = _filesModel->getCompareOptions();
                options.mode = CompareMode::Split;
                _filesModel->setCompareOptions(options);
            });
        addItem(_actions["Compare/Split"]);

        _actions["Compare/Overlay"] = std::make_shared<dtk::Action>(
            "Overlay",
            dtk::Key::O,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this](bool)
            {
                CompareOptions options = _filesModel->getCompareOptions();
                options.mode = CompareMode::Overlay;
                _filesModel->setCompareOptions(options);
            });
        addItem(_actions["Compare/Overlay"]);

        _actions["Compare/Horizontal"] = std::make_shared<dtk::Action>(
            "Horizontal",
            dtk::Key::H,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this](bool)
            {
                CompareOptions options = _filesModel->getCompareOptions();
                options.mode = CompareMode::Horizontal;
                _filesModel->setCompareOptions(options);
            });
        addItem(_actions["Compare/Horizontal"]);

        _actions["Compare/Vertical"] = std::make_shared<dtk::Action>(
            "Vertical",
            dtk::Key::V,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this](bool)
            {
                CompareOptions options = _filesModel->getCompareOptions();
                options.mode = CompareMode::Vertical;
                _filesModel->setCompareOptions(options);
            });
        addItem(_actions["Compare/Vertical"]);

        _modeObserver = dtk::ValueObserver<CompareOptions>::create(
            app->getFilesModel()->observeCompareOptions(),
            [this](const CompareOptions& value)
            {
                setItemChecked(_actions["Compare/A"], CompareMode::A == value.mode);
                setItemChecked(_actions["Compare/B"], CompareMode::B == value.mode);
                setItemChecked(_actions["Compare/Split"], CompareMode::Split == value.mode);
                setItemChecked(_actions["Compare/Overlay"], CompareMode::Overlay == value.mode);
                setItemChecked(_actions["Compare/Horizontal"], CompareMode::Horizontal == value.mode);
                setItemChecked(_actions["Compare/Vertical"], CompareMode::Vertical == value.mode);
            });

        _filesObserver = dtk::ListObserver<std::shared_ptr<File> >::create(
            _filesModel->observeFiles(),
            [this](const std::vector<std::shared_ptr<File> >& files)
            {
                _menus["BFile"]->clear();
                _bFileActions.clear();
                for (int i = 0; i < files.size(); ++i)
                {
                    auto item = std::make_shared<dtk::Action>(
                        files[i]->getPath().filename().string(),
                        [this, i](bool value)
                        {
                            _filesModel->setBIndex(value ? i : -1);
                            close();
                        });
                    _menus["BFile"]->addItem(item);
                    _bFileActions.push_back(item);
                }
            });

        _bIndexObserver = dtk::ValueObserver<int>::create(
            _filesModel->observeBIndex(),
            [this](int index)
            {
                for (int i = 0; i < _bFileActions.size(); ++i)
                {
                    _menus["BFile"]->setItemChecked(_bFileActions[i], i == index);
                }
            });
    }

    CompareMenu::~CompareMenu()
    {}

    std::shared_ptr<CompareMenu> CompareMenu::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<CompareMenu>(new CompareMenu);
        out->_init(context, app, parent);
        return out;
    }

    const std::map<std::string, std::shared_ptr<dtk::Action> >& CompareMenu::getActions() const
    {
        return _actions;
    }
}
