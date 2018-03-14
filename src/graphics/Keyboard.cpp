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

#include "Keyboard.hpp"

static bool readMap(const std::unordered_map<irr::EKEY_CODE,bool>& map, irr::EKEY_CODE key) {
    const auto it = map.find(key);
    bool result = false;
    if (it != map.end()) {
        result = it->second;
    }
    return result;
}

bool Keyboard::isKeyDown(irr::EKEY_CODE key) const {
    return readMap(keyDown, key);
}

bool Keyboard::isKeyMovingDown(irr::EKEY_CODE key) const {
    return readMap(keyMoving, key) && readMap(keyDown, key);
}

bool Keyboard::isKeyMovingUp(irr::EKEY_CODE key) const {
    return readMap(keyMoving, key) && !readMap(keyDown, key);
}

void Keyboard::OnEvent(const irr::SEvent::SKeyInput& event) {
    keyMoving[event.Key] = (keyDown[event.Key] != event.PressedDown);
    keyDown[event.Key] = event.PressedDown;
}

void Keyboard::newFrame() {
    keyMoving.clear();
}
