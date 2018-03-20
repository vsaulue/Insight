/*
 * This file is part of Insight.
 * Copyright (C) 2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#ifndef KEYINPUTEVENT_HPP
#define KEYINPUTEVENT_HPP

#include "EventReport.hpp"
#include "InputEvent.hpp"

/** Class representing a keyboard input from the user. */
class KeyInputEvent : public InputEvent {
public:

    /** Possible key event that can be watched. */
    enum class KeyEvent {
        ButtonMovingDown, /**< The key was just pressed. */
        ButtonDown, /**< The key is currently pressed. */
        ButtonMovingUp /**< The key was just released. */
    };

    /**
     * Creates a new input event.
     * @param key
     * @param event
     */
    KeyInputEvent(irr::EKEY_CODE key, KeyEvent event);

    EventReport eventReport(const Keyboard& keyboard) override;

private:
    /** Key watched by this event. */
    irr::EKEY_CODE key;
    /** Key state watched by this event. */
    KeyEvent event;
};

#endif /* KEYINPUTEVENT_HPP */
