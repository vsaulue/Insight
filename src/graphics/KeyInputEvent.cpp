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

#include <algorithm>

#include "KeyInputEvent.hpp"

KeyInputEvent::KeyInputEvent(irr::EKEY_CODE key, KeyEvent event) : key(key), event(event) {

}

EventReport KeyInputEvent::eventReport(const Keyboard& keyboard, const Mouse& mouse) {
    EventReport result = {false, 0.0f};
    switch (event) {
    case KeyEvent::ButtonMovingDown:
        result.happened = keyboard.isKeyMovingDown(key);
        break;
    case KeyEvent::ButtonDown:
        result.happened = keyboard.isKeyDown(key);
        break;
    case KeyEvent::ButtonMovingUp:
        result.happened = keyboard.isKeyMovingUp(key);
        break;
    }
    if (result.happened) {
        result.intensity = 1.0f;
    }
    return result;
}

const std::string& KeyInputEvent::getInputName() const {
    const auto& map = keyNameMap();
    auto it = std::find_if(map.begin(), map.end(), [this](auto& pair) -> bool { return pair.first == this->key;});
    if (it == map.end()) {
        static const std::string unknownKey = "Unkown key name";
        return unknownKey;
    } else {
        return it->second;
    }
}

const std::map<irr::EKEY_CODE, std::string>& KeyInputEvent::keyNameMap() {
    static const std::map<irr::EKEY_CODE, std::string> result = {
        // Letters
        {irr::KEY_KEY_A, "KEY_A"},
        {irr::KEY_KEY_B, "KEY_B"},
        {irr::KEY_KEY_C, "KEY_C"},
        {irr::KEY_KEY_D, "KEY_D"},
        {irr::KEY_KEY_E, "KEY_E"},
        {irr::KEY_KEY_F, "KEY_F"},
        {irr::KEY_KEY_G, "KEY_G"},
        {irr::KEY_KEY_H, "KEY_H"},
        {irr::KEY_KEY_I, "KEY_I"},
        {irr::KEY_KEY_J, "KEY_J"},
        {irr::KEY_KEY_K, "KEY_K"},
        {irr::KEY_KEY_L, "KEY_L"},
        {irr::KEY_KEY_M, "KEY_M"},
        {irr::KEY_KEY_N, "KEY_N"},
        {irr::KEY_KEY_O, "KEY_O"},
        {irr::KEY_KEY_P, "KEY_P"},
        {irr::KEY_KEY_Q, "KEY_Q"},
        {irr::KEY_KEY_R, "KEY_R"},
        {irr::KEY_KEY_S, "KEY_S"},
        {irr::KEY_KEY_T, "KEY_T"},
        {irr::KEY_KEY_U, "KEY_U"},
        {irr::KEY_KEY_V, "KEY_V"},
        {irr::KEY_KEY_W, "KEY_W"},
        {irr::KEY_KEY_X, "KEY_X"},
        {irr::KEY_KEY_Y, "KEY_Y"},
        {irr::KEY_KEY_Z, "KEY_Z"},
        // 0->9 (Not the numpad)
        {irr::KEY_KEY_0, "KEY_0"},
        {irr::KEY_KEY_1, "KEY_1"},
        {irr::KEY_KEY_2, "KEY_2"},
        {irr::KEY_KEY_3, "KEY_3"},
        {irr::KEY_KEY_4, "KEY_4"},
        {irr::KEY_KEY_5, "KEY_5"},
        {irr::KEY_KEY_6, "KEY_6"},
        {irr::KEY_KEY_7, "KEY_7"},
        {irr::KEY_KEY_8, "KEY_8"},
        {irr::KEY_KEY_9, "KEY_9"},
        // F1->F12
        {irr::KEY_F1, "KEY_F1"},
        {irr::KEY_F2, "KEY_F2"},
        {irr::KEY_F3, "KEY_F3"},
        {irr::KEY_F4, "KEY_F4"},
        {irr::KEY_F5, "KEY_F5"},
        {irr::KEY_F6, "KEY_F6"},
        {irr::KEY_F7, "KEY_F7"},
        {irr::KEY_F8, "KEY_F8"},
        {irr::KEY_F9, "KEY_F9"},
        {irr::KEY_F10, "KEY_F10"},
        {irr::KEY_F11, "KEY_F11"},
        {irr::KEY_F12, "KEY_F12"},
        // some special characters
        {irr::KEY_LSHIFT, "KEY_LeftShift"},
        {irr::KEY_LCONTROL, "KEY_LeftCtrl"},
        {irr::KEY_TAB, "KEY_Tab"},
        {irr::KEY_LMENU, "KEY_LeftAlt"},
        {irr::KEY_SPACE, "KEY_Space"},
    };
    return result;
}

std::unique_ptr<KeyInputEvent> KeyInputEvent::makeByName(const std::string& name, bool persistent) {
    std::unique_ptr<KeyInputEvent> result(nullptr);
    const auto& map = keyNameMap();
    auto it = std::find_if(map.begin(), map.end(), [&name](auto& pair) -> bool { return pair.second == name; });
    if (it != map.end()) {
        KeyEvent event;
        if (persistent) {
            event = KeyEvent::ButtonDown;
        } else {
            event = KeyEvent::ButtonMovingDown;
        }
        result = std::make_unique<KeyInputEvent>(it->first, event);
    }
    return result;
}
