// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/Divider.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Splitter.h>
#include <dtk/ui/TabWidget.h>

namespace toucan
{
    class App;
    class BottomBar;
    class Document;
    class TimelineView;
    class Viewport;

    class DocumentTab : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~DocumentTab();

        static std::shared_ptr<DocumentTab> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<Document>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::shared_ptr<dtk::Splitter> _vSplitter;
        std::shared_ptr<dtk::Splitter> _hSplitter;
        std::shared_ptr<Viewport> _viewport;
        std::shared_ptr<dtk::VerticalLayout> _bottomLayout;
        std::shared_ptr<BottomBar> _bottomBar;
        std::shared_ptr<TimelineView> _timelineView;
    };
}

