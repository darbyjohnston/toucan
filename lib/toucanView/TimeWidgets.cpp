// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TimeWidgets.h"

#include <dtk/ui/ToolButton.h>

#include <sstream>

namespace toucan
{
    void FrameButtons::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::FrameButtons", parent);

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        auto startButton = dtk::ToolButton::create(context, _layout);
        startButton->setIcon("FrameStart");
        startButton->setTooltip("Go to the start frame");

        auto prevButton = dtk::ToolButton::create(context, _layout);
        prevButton->setIcon("FramePrev");
        prevButton->setRepeatClick(true);
        prevButton->setTooltip("Go to the previous frame");

        auto nextButton = dtk::ToolButton::create(context, _layout);
        nextButton->setIcon("FrameNext");
        nextButton->setRepeatClick(true);
        nextButton->setTooltip("Go to the next frame");

        auto endButton = dtk::ToolButton::create(context, _layout);
        endButton->setIcon("FrameEnd");
        endButton->setTooltip("Go to the end frame");

        _buttonGroup = dtk::ButtonGroup::create(context, dtk::ButtonGroupType::Click);
        _buttonGroup->addButton(startButton);
        _buttonGroup->addButton(prevButton);
        _buttonGroup->addButton(nextButton);
        _buttonGroup->addButton(endButton);

        _buttonGroup->setClickedCallback(
            [this](int index)
            {
                if (_callback)
                {
                    _callback(static_cast<TimeAction>(index));
                }
            });
    }

    FrameButtons::~FrameButtons()
    {}

    std::shared_ptr<FrameButtons> FrameButtons::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<FrameButtons>(new FrameButtons);
        out->_init(context, parent);
        return out;
    }

    void FrameButtons::setCallback(const std::function<void(TimeAction)>& value)
    {
        _callback = value;
    }

    void FrameButtons::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void FrameButtons::sizeHintEvent(const dtk::SizeHintEvent& value)
    {
        IWidget::sizeHintEvent(value);
        _setSizeHint(_layout->getSizeHint());
    }

    void PlaybackButtons::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::PlaybackButtons", parent);

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        auto reverseButton = dtk::ToolButton::create(context, _layout);
        reverseButton->setIcon("PlaybackReverse");
        reverseButton->setTooltip("Reverse playback");

        auto stopButton = dtk::ToolButton::create(context, _layout);
        stopButton->setIcon("PlaybackStop");
        stopButton->setTooltip("Stop playback");

        auto forwardButton = dtk::ToolButton::create(context, _layout);
        forwardButton->setIcon("PlaybackForward");
        forwardButton->setTooltip("Forward playback");

        _buttonGroup = dtk::ButtonGroup::create(context, dtk::ButtonGroupType::Radio);
        _buttonGroup->addButton(stopButton);
        _buttonGroup->addButton(forwardButton);
        _buttonGroup->addButton(reverseButton);

        _playbackUpdate();

        _buttonGroup->setCheckedCallback(
            [this](int index, bool value)
            {
                if (value)
                {
                    if (_callback)
                    {
                        _callback(static_cast<Playback>(index));
                    }
                }
            });
    }

    PlaybackButtons::~PlaybackButtons()
    {}

    std::shared_ptr<PlaybackButtons> PlaybackButtons::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<PlaybackButtons>(new PlaybackButtons);
        out->_init(context, parent);
        return out;
    }

    void PlaybackButtons::setPlayback(Playback value)
    {
        if (value == _playback)
            return;
        _playback = value;
        _playbackUpdate();
    }

    void PlaybackButtons::setCallback(const std::function<void(Playback)>& value)
    {
        _callback = value;
    }

    void PlaybackButtons::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void PlaybackButtons::sizeHintEvent(const dtk::SizeHintEvent& value)
    {
        IWidget::sizeHintEvent(value);
        _setSizeHint(_layout->getSizeHint());
    }

    void PlaybackButtons::_playbackUpdate()
    {
        _buttonGroup->setChecked(static_cast<int>(_playback), true);
    }

    void TimeEdit::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::TimeEdit", parent);

        _timeUnitsModel = timeUnitsModel;

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _lineEdit = dtk::LineEdit::create(context, _layout);
        _lineEdit->setFontRole(dtk::FontRole::Mono);
        _lineEdit->setFormat("00:00:00:00");

        _incButtons = dtk::IncButtons::create(context, _layout);

        _timeUpdate();

        _lineEdit->setTextCallback(
            [this](const std::string& text)
            {
                if (_callback)
                {
                    _callback(_timeUnitsModel->fromString(text, _time.rate()));
                }
            });

        _incButtons->setIncCallback(
            [this]
            {
                _timeInc();
            });
        _incButtons->setDecCallback(
            [this]
            {
                _timeDec();
            });

        _timeUnitsObserver = dtk::ValueObserver<TimeUnits>::create(
            timeUnitsModel->observeTimeUnits(),
            [this](TimeUnits)
            {
                _timeUpdate();
            });
    }

    TimeEdit::~TimeEdit()
    {}

    std::shared_ptr<TimeEdit> TimeEdit::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<TimeEdit>(new TimeEdit);
        out->_init(context, timeUnitsModel, parent);
        return out;
    }

    void TimeEdit::setTime(const OTIO_NS::RationalTime& value)
    {
        if (value.strictly_equal(_time))
            return;
        _time = value;
        _timeUpdate();
    }

    void TimeEdit::setTimeRange(const OTIO_NS::TimeRange& value)
    {
        if (value.start_time().strictly_equal(_timeRange.start_time()) &&
            value.duration().strictly_equal(_timeRange.duration()))
            return;
        _timeRange = value;
        _timeUpdate();
    }

    void TimeEdit::setCallback(const std::function<void(const OTIO_NS::RationalTime&)>& value)
    {
        _callback = value;
    }

    void TimeEdit::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void TimeEdit::sizeHintEvent(const dtk::SizeHintEvent& value)
    {
        IWidget::sizeHintEvent(value);
        _setSizeHint(_layout->getSizeHint());
    }

    void TimeEdit::keyPressEvent(dtk::KeyEvent& event)
    {
        if (_lineEdit->hasKeyFocus())
        {
            if (0 == event.modifiers)
            {
                switch (event.key)
                {
                case dtk::Key::Up:
                case dtk::Key::Right:
                    event.accept = true;
                    _timeInc();
                    break;
                case dtk::Key::Down:
                case dtk::Key::Left:
                    event.accept = true;
                    _timeDec();
                    break;
                case dtk::Key::PageUp:
                    event.accept = true;
                    _timeInc(_time.rate());
                    break;
                case dtk::Key::PageDown:
                    event.accept = true;
                    _timeInc(-_time.rate());
                    break;
                default: break;
                }
            }
        }
        if (!event.accept)
        {
            IWidget::keyPressEvent(event);
        }
    }

    void TimeEdit::keyReleaseEvent(dtk::KeyEvent& event)
    {
        IWidget::keyReleaseEvent(event);
        event.accept = true;
    }

    void TimeEdit::_timeUpdate()
    {
        _lineEdit->setText(_timeUnitsModel->toString(_time));
    }

    void TimeEdit::_timeInc(int value)
    {
        if (_callback)
        {
            const auto time = _time + OTIO_NS::RationalTime(value, _time.rate());
            _callback(time);
        }
    }

    void TimeEdit::_timeDec(int value)
    {
        if (_callback)
        {
            const auto time = _time - OTIO_NS::RationalTime(value, _time.rate());
            _callback(time);
        }
    }

    void TimeLabel::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        IWidget::_init(context, "toucan::TimeLabel", parent);

        _timeUnitsModel = timeUnitsModel;

        _label = dtk::Label::create(context, shared_from_this());
        _label->setFontRole(dtk::FontRole::Mono);
        _label->setMarginRole(dtk::SizeRole::MarginInside);

        _timeUpdate();

        _timeUnitsObserver = dtk::ValueObserver<TimeUnits>::create(
            timeUnitsModel->observeTimeUnits(),
            [this](TimeUnits value)
            {
                _timeUpdate();
            });
    }

    TimeLabel::~TimeLabel()
    {}

    std::shared_ptr<TimeLabel> TimeLabel::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<TimeLabel>(new TimeLabel);
        out->_init(context, timeUnitsModel, parent);
        return out;
    }

    void TimeLabel::setTime(const OTIO_NS::RationalTime& value)
    {
        if (value.strictly_equal(_time))
            return;
        _time = value;
        _timeUpdate();
    }

    void TimeLabel::setMarginRole(dtk::SizeRole value)
    {
        _label->setMarginRole(value);
    }

    void TimeLabel::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _label->setGeometry(value);
    }

    void TimeLabel::sizeHintEvent(const dtk::SizeHintEvent& value)
    {
        IWidget::sizeHintEvent(value);
        _setSizeHint(_label->getSizeHint());
    }

    void TimeLabel::_timeUpdate()
    {
        _label->setText(_timeUnitsModel->toString(_time));
    }
}
