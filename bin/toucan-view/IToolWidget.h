// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/IWidget.h>

namespace toucan
{
    class App;

    class IToolWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::string& objectName,
            const std::string& text,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~IToolWidget() = 0;

        const std::string& getText() const;

    protected:
        std::weak_ptr<App> _app;
        std::string _text;
    };
}

