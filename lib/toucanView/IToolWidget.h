// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <ftk/Ui/IWidget.h>

namespace toucan
{
    class App;

    //! Base class for tools.
    class IToolWidget : public ftk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            const std::shared_ptr<App>&,
            const std::string& objectName,
            const std::string& text,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~IToolWidget() = 0;

        //! Get the tool text.
        const std::string& getText() const;

    protected:
        std::weak_ptr<App> _app;
        std::string _text;
    };
}

