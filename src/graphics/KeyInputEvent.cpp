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
