// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ViewModel.h"

#include <nlohmann/json.hpp>

#include <sstream>

namespace toucan
{
    ViewModel::ViewModel(const std::shared_ptr<dtk::Context>& context)
    {
        _zoomIn = dtk::ObservableValue<bool>::create(false);
        _zoomOut = dtk::ObservableValue<bool>::create(false);
        _zoomReset = dtk::ObservableValue<bool>::create(false);
        _frameView = dtk::ObservableValue<bool>::create(true);
    }

    ViewModel::~ViewModel()
    {}

    void ViewModel::zoomIn()
    {
        _zoomIn->setAlways(true);
    }

    void ViewModel::zoomOut()
    {
        _zoomOut->setAlways(true);
    }

    void ViewModel::zoomReset()
    {
        _zoomReset->setAlways(true);
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeZoomIn() const
    {
        return _zoomIn;
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeZoomOut() const
    {
        return _zoomOut;
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeZoomReset() const
    {
        return _zoomReset;
    }

    bool ViewModel::getFrameView() const
    {
        return _frameView->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeFrameView() const
    {
        return _frameView;
    }

    void ViewModel::setFrameView(bool value)
    {
        _frameView->setIfChanged(value);
    }

    GlobalViewModel::GlobalViewModel(const std::shared_ptr<dtk::Context>& context)
    {
        _settings = context->getSystem<dtk::Settings>();
        bool hud = false;
        try
        {
            const auto json = std::any_cast<nlohmann::json>(_settings->get("GlobalViewModel"));
            auto i = json.find("HUD");
            if (i != json.end() && i->is_boolean())
            {
                hud = i->get<bool>();
            }
        }
        catch (const std::exception&)
        {}

        _hud = dtk::ObservableValue<bool>::create(hud);
    }

    GlobalViewModel::~GlobalViewModel()
    {
        nlohmann::json json;
        json["HUD"] = _hud->get();
        _settings->set("GlobalViewModel", json);
    }

    bool GlobalViewModel::getHUD() const
    {
        return _hud->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > GlobalViewModel::observeHUD() const
    {
        return _hud;
    }

    void GlobalViewModel::setHUD(bool value)
    {
        _hud->setIfChanged(value);
    }
}
