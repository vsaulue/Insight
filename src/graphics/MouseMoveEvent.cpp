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

#include <algorithm>
#include <cassert>

#include "MouseMoveEvent.hpp"

static const float MOUSE_MOVE_MULTIPLIER = 0.1f;

EventReport MouseMoveEvent::eventReport(const Keyboard& keyboard, const Mouse& mouse) {
    irr::core::position2d<irr::s32> delta = mouse.getMove();
    EventReport result = {false, 0.0f};
    if (!(mouseLockedOnly && !mouse.isMouseLocked())) {
        float intensity = 0;
        switch(direction) {
        case MouseDirection::Left:
            intensity = -delta.X;
            break;
        case MouseDirection::Right:
            intensity = delta.X;
            break;
        case MouseDirection::Up:
            intensity = -delta.Y;
            break;
        case MouseDirection::Down:
            intensity = delta.Y;
            break;
        }
        if (intensity > 0) {
            result.happened = true;
            result.intensity = intensity * MOUSE_MOVE_MULTIPLIER;
        }
    }
    return result;
}

const std::string& MouseMoveEvent::getInputName() const {
    const auto& map = directionNameMap();
    auto it = std::find_if(map.begin(), map.end(), [this](auto& pair) -> bool { return pair.first == this->direction; });
    assert(it != map.end());
    return it->second;
}

const std::map<MouseMoveEvent::MouseDirection, std::string>& MouseMoveEvent::directionNameMap() {
    using MD = MouseDirection;
    static const std::map<MouseDirection, std::string> result {
        {MD::Left, "MOUSE_MoveLeft"},
        {MD::Right, "MOUSE_MoveRight"},
        {MD::Up, "MOUSE_MoveUp"},
        {MD::Down, "MOUSE_MoveDown"}
    };
    return result;
}

std::unique_ptr<MouseMoveEvent> MouseMoveEvent::makeByName(const std::string& name, bool persistent) {
    std::unique_ptr<MouseMoveEvent> result(nullptr);
    const auto& map = directionNameMap();
    auto it = std::find_if(map.begin(), map.end(), [&name](auto& pair) -> bool { return pair.second == name; });
    if (it != map.end()) {
        result = std::make_unique<MouseMoveEvent>(it->first);
    }
    return result;
}
