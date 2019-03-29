/*
 * This file is part of Insight.
 * Copyright (C) 2018-2019 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
 *
 * Insight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Insight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Insight.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MOUSEMOVEEVENT_HPP
#define MOUSEMOVEEVENT_HPP

#include <map>
#include <memory>

#include "EventReport.hpp"
#include "InputEvent.hpp"

/** Input event that monitors mouse movements. */
class MouseMoveEvent : public InputEvent {
public:
    /** Possible mouse directions watched by a MouseMoveEvent. */
    enum class MouseDirection {
        Left,
        Right,
        Up,
        Down
    };

    /**
     * Creates a new Input event watching mouse movements.
     *
     * @param[in] direction Direction that should trigger this event.
     * @param[in] mouseLockedOnly Event can be triggered only if the mouse is locked in the middle of the screen.
     */
    MouseMoveEvent(MouseDirection direction, bool mouseLockedOnly = true) :
        direction(direction),
        mouseLockedOnly(mouseLockedOnly)
    {

    }

    EventReport eventReport(const Keyboard& keyboard, const Mouse& mouse) override;

    const std::string& getInputName() const override;

    /**
     * Gets a map of all mouse directions names.
     *
     * @return A map giving a direction name from its enum code.
     */
    static const std::map<MouseDirection, std::string>& directionNameMap();

    /**
     * Make a MouseMoveEvent from an input name.
     *
     * See InputEventFactory::makeByName().
     *
     * @param[in] name Name of the input.
     * @param[in] persistent True for a persistent event, false for an instant event.
     * @return A pointer to the new event, or nullptr if the name is invalid.
     */
    static std::unique_ptr<MouseMoveEvent> makeByName(const std::string& name, bool persistent);
private:
    /** Direction of the mouse that should trigger this event. */
    MouseDirection direction;
    /** Event can be triggered only if the mouse is locked in the middle of the screen. */
    bool mouseLockedOnly;
};

#endif /* MOUSEMOVEEVENT_HPP */
