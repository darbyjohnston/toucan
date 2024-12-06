// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "WindowModel.h"

#include <dtk/ui/Settings.h>
#include <dtk/core/Error.h>
#include <dtk/core/String.h>

#include <nlohmann/json.hpp>

#include <sstream>

namespace toucan
{
    DTK_ENUM_IMPL(
        WindowComponent,
        "ToolBar",
        "ToolsPanel",
        "PlaybackPanel");

    WindowModel::WindowModel(const std::shared_ptr<dtk::Context>& context)
    {
        _context = context;

        std::map<WindowComponent, bool> components =
        {
            { WindowComponent::ToolBar, true },
            { WindowComponent::ToolsPanel, true },
            { WindowComponent::PlaybackPanel, true }
        };
        bool tooltips = true;
        try
        {
            auto settings = context->getSystem<dtk::Settings>();
            const auto json = std::any_cast<nlohmann::json>(settings->get("WindowModel"));
            for (auto& i : components)
            {
                std::stringstream ss;
                ss << i.first;
                auto j = json.find(ss.str());
                if (j != json.end() && j->is_boolean())
                {
                    i.second = j->get<bool>();
                }
            }
            auto i = json.find("Tooltips");
            if (i != json.end() && i->is_boolean())
            {
                tooltips = i->get<bool>();
            }
        }
        catch (const std::exception&)
        {}

        _components = dtk::ObservableMap<WindowComponent, bool>::create(components);
        _tooltips = dtk::ObservableValue<bool>::create(tooltips);
    }

    WindowModel::~WindowModel()
    {
        nlohmann::json json;
        for (const auto i : _components->get())
        {
            std::stringstream ss;
            ss << i.first;
            json[ss.str()] = i.second;
        }
        json["Tooltips"] = _tooltips->get();
        auto context = _context.lock();
        auto settings = context->getSystem<dtk::Settings>();
        settings->set("WindowModel", json);
    }

    const std::map<WindowComponent, bool> WindowModel::getComponents() const
    {
        return _components->get();
    }

    std::shared_ptr<dtk::IObservableMap<WindowComponent, bool> > WindowModel::observeComponents() const
    {
        return _components;
    }

    void WindowModel::setComponents(const std::map<WindowComponent, bool>& value)
    {
        _components->setIfChanged(value);
    }

    bool WindowModel::getComponent(WindowComponent value) const
    {
        return _components->getItem(value);
    }

    void WindowModel::setComponent(WindowComponent component, bool value)
    {
        _components->setItemOnlyIfChanged(component, value);
    }

    bool WindowModel::getTooltips() const
    {
        return _tooltips->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > WindowModel::observeTooltips() const
    {
        return _tooltips;
    }

    void WindowModel::setTooltips(bool value)
    {
        _tooltips->setIfChanged(value);
    }
}
