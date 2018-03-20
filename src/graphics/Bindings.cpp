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

#include "Bindings.hpp"
#include "KeyInputEvent.hpp"
#include "MouseMoveEvent.hpp"

Bindings::Bindings() {
    using KE = KeyInputEvent::KeyEvent;
    using MD = MouseMoveEvent::MouseDirection;
    actionToKey[Action::CameraForward] = std::make_unique<KeyInputEvent>(irr::KEY_KEY_W, KE::ButtonDown);
    actionToKey[Action::CameraBackward] = std::make_unique<KeyInputEvent>(irr::KEY_KEY_S, KE::ButtonDown);
    actionToKey[Action::CameraLeft] = std::make_unique<KeyInputEvent>(irr::KEY_KEY_A, KE::ButtonDown);
    actionToKey[Action::CameraRight] = std::make_unique<KeyInputEvent>(irr::KEY_KEY_D, KE::ButtonDown);
    actionToKey[Action::CameraUp] = std::make_unique<KeyInputEvent>(irr::KEY_LSHIFT, KE::ButtonDown);
    actionToKey[Action::CameraDown] = std::make_unique<KeyInputEvent>(irr::KEY_LCONTROL, KE::ButtonDown);
    actionToKey[Action::ToggleMouseLock] = std::make_unique<KeyInputEvent>(irr::KEY_KEY_Q, KE::ButtonMovingDown);
    actionToKey[Action::CameraTurnLeft] = std::make_unique<MouseMoveEvent>(MD::Left);
    actionToKey[Action::CameraTurnRight] = std::make_unique<MouseMoveEvent>(MD::Right);
    actionToKey[Action::CameraTurnUp] = std::make_unique<MouseMoveEvent>(MD::Up);
    actionToKey[Action::CameraTurnDown] = std::make_unique<MouseMoveEvent>(MD::Down);
}

int Bindings::luaIndex(const std::string& memberName, LuaStateView& state) {
    return 0;
}
