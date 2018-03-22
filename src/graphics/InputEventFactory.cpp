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

#include "KeyInputEvent.hpp"
#include "InputEventFactory.hpp"
#include "MouseMoveEvent.hpp"

using EventMaker = InputEventFactory::EventMaker;

/**
 * Creates an EventMaker generating events for a specific key.
 *
 * Persistent events will be triggered as long as the key is held down. Non persistent
 * events once when the button is pressed.
 *
 * @param[in] key Key that should trigger the generated event.
 * @return An EventMaker creating events for the given key.
 */
static EventMaker keyEventMaker(irr::EKEY_CODE key) {
    using KeyEvent = KeyInputEvent::KeyEvent;
    return [key](bool persistent) {
        KeyEvent eventType;
        if (persistent) {
            eventType = KeyEvent::ButtonDown;
        } else {
            eventType = KeyEvent::ButtonMovingDown;
        }
        return std::make_unique<KeyInputEvent>(key, eventType);
    };
}

/**
 * Creates an EventMaker generating events for a specific mouse movement.
 *
 * @param[in] direction Mouse movement triggering the generated events.
 * @return An EventMaker creating events for the given mouse movement.
 */
static EventMaker mouseMoveEventMaker(MouseMoveEvent::MouseDirection direction) {
    return [direction](bool persistent) {
        return std::make_unique<MouseMoveEvent>(direction);
    };
}

const std::map<std::string, InputEventFactory::EventMaker>& InputEventFactory::list() {
    using MD = MouseMoveEvent::MouseDirection;
    static const std::map<std::string, EventMaker> result = {
        // Keyboard: A->Z
        {"KEY_A", keyEventMaker(irr::KEY_KEY_A)},
        {"KEY_B", keyEventMaker(irr::KEY_KEY_B)},
        {"KEY_C", keyEventMaker(irr::KEY_KEY_C)},
        {"KEY_D", keyEventMaker(irr::KEY_KEY_D)},
        {"KEY_E", keyEventMaker(irr::KEY_KEY_E)},
        {"KEY_F", keyEventMaker(irr::KEY_KEY_F)},
        {"KEY_G", keyEventMaker(irr::KEY_KEY_G)},
        {"KEY_H", keyEventMaker(irr::KEY_KEY_H)},
        {"KEY_I", keyEventMaker(irr::KEY_KEY_I)},
        {"KEY_J", keyEventMaker(irr::KEY_KEY_J)},
        {"KEY_K", keyEventMaker(irr::KEY_KEY_K)},
        {"KEY_L", keyEventMaker(irr::KEY_KEY_L)},
        {"KEY_M", keyEventMaker(irr::KEY_KEY_M)},
        {"KEY_N", keyEventMaker(irr::KEY_KEY_N)},
        {"KEY_O", keyEventMaker(irr::KEY_KEY_O)},
        {"KEY_P", keyEventMaker(irr::KEY_KEY_P)},
        {"KEY_Q", keyEventMaker(irr::KEY_KEY_Q)},
        {"KEY_R", keyEventMaker(irr::KEY_KEY_R)},
        {"KEY_S", keyEventMaker(irr::KEY_KEY_S)},
        {"KEY_T", keyEventMaker(irr::KEY_KEY_T)},
        {"KEY_U", keyEventMaker(irr::KEY_KEY_U)},
        {"KEY_V", keyEventMaker(irr::KEY_KEY_V)},
        {"KEY_W", keyEventMaker(irr::KEY_KEY_W)},
        {"KEY_X", keyEventMaker(irr::KEY_KEY_X)},
        {"KEY_Y", keyEventMaker(irr::KEY_KEY_Y)},
        {"KEY_Z", keyEventMaker(irr::KEY_KEY_Z)},
        // Keyboard: 0->9 (Not the numpad)
        {"KEY_0", keyEventMaker(irr::KEY_KEY_0)},
        {"KEY_1", keyEventMaker(irr::KEY_KEY_1)},
        {"KEY_2", keyEventMaker(irr::KEY_KEY_2)},
        {"KEY_3", keyEventMaker(irr::KEY_KEY_3)},
        {"KEY_4", keyEventMaker(irr::KEY_KEY_4)},
        {"KEY_5", keyEventMaker(irr::KEY_KEY_5)},
        {"KEY_6", keyEventMaker(irr::KEY_KEY_6)},
        {"KEY_7", keyEventMaker(irr::KEY_KEY_7)},
        {"KEY_8", keyEventMaker(irr::KEY_KEY_8)},
        {"KEY_9", keyEventMaker(irr::KEY_KEY_9)},
        // Keyboard: F1->F12
        {"KEY_F1", keyEventMaker(irr::KEY_F1)},
        {"KEY_F2", keyEventMaker(irr::KEY_F2)},
        {"KEY_F3", keyEventMaker(irr::KEY_F3)},
        {"KEY_F4", keyEventMaker(irr::KEY_F4)},
        {"KEY_F5", keyEventMaker(irr::KEY_F5)},
        {"KEY_F6", keyEventMaker(irr::KEY_F6)},
        {"KEY_F7", keyEventMaker(irr::KEY_F7)},
        {"KEY_F8", keyEventMaker(irr::KEY_F8)},
        {"KEY_F9", keyEventMaker(irr::KEY_F9)},
        {"KEY_F10", keyEventMaker(irr::KEY_F10)},
        {"KEY_F11", keyEventMaker(irr::KEY_F11)},
        {"KEY_F12", keyEventMaker(irr::KEY_F12)},
        // Keyboard: some special characters
        {"KEY_LeftShift", keyEventMaker(irr::KEY_LSHIFT)},
        {"KEY_LeftCtrl", keyEventMaker(irr::KEY_LCONTROL)},
        {"KEY_Tab", keyEventMaker(irr::KEY_TAB)},
        {"KEY_LeftAlt", keyEventMaker(irr::KEY_LMENU)},
        {"KEY_Space", keyEventMaker(irr::KEY_SPACE)},
        // Mouse: cursor movement
        {"MOUSE_MoveLeft", mouseMoveEventMaker(MD::Left)},
        {"MOUSE_MoveRight", mouseMoveEventMaker(MD::Right)},
        {"MOUSE_MoveUp", mouseMoveEventMaker(MD::Up)},
        {"MOUSE_MoveDown", mouseMoveEventMaker(MD::Down)},
    };
    return result;
}

std::unique_ptr<InputEvent> InputEventFactory::makeByName(const std::string& name, bool persistent) {
    std::unique_ptr<InputEvent> result(nullptr);
    const auto& map = list();
    auto it = map.find(name);
    if (it != map.end()) {
        result = it->second(persistent);
    }
    return result;
}
