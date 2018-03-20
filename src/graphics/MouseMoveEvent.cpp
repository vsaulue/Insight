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

#include "MouseMoveEvent.hpp"

static const float MOUSE_MOVE_MULTIPLIER = 0.1f;

EventReport MouseMoveEvent::eventReport(const Keyboard& keyboard, const Mouse& mouse) {
    irr::core::position2d<irr::s32> delta = mouse.getMove();
    EventReport result = {false, 0.0f};
    if (!(mouseLockedOnly && !mouse.isMouseLocked())) {
        float intensity;
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
