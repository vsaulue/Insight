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

#include <utility>

#include "ActionInfo.hpp"
#include "Bindings.hpp"
#include "InputEventFactory.hpp"
#include "KeyInputEvent.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/LuaException.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaNativeString.hpp"
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
    using Method = LuaMethod<Bindings>;
    if (memberName == "listActions") {
        state.push<Method>([](Bindings& obj, LuaStateView& state) -> int {
            const auto& set = ActionInfo::list();
            int count = set.size();
            state.checkStack(count);
            for (const ActionInfo& info : set) {
                state.push<LuaNativeString>(info.name.c_str());
            }
            return count;
        });
        return 1;
    } else if (memberName == "listEvents") {
        state.push<Method>([](Bindings& obj, LuaStateView& state) -> int {
            const auto& map = InputEventFactory::list();
            int count = map.size();
            state.checkStack(count);
            for (const auto& pair : map) {
                state.push<LuaNativeString>(pair.first.c_str());
            }
            return count;
        });
        return 1;
    } else if (memberName == "set") {
        state.push<Method>([](Bindings& obj, LuaStateView& state) -> int {
            std::string actionName(state.get<LuaNativeString>(2));
            std::string eventName(state.get<LuaNativeString>(3));
            const ActionInfo* actionInfo = ActionInfo::byName(actionName);
            if (actionInfo == nullptr) {
                std::string errorMsg = std::string("Invalid action name: ") + actionName;
                throw LuaException(errorMsg.c_str());
            }
            std::unique_ptr<InputEvent> event = InputEventFactory::makeByName(eventName, actionInfo->persistent);
            if (event == nullptr) {
                std::string errorMsg = std::string("Invalid event name: ") + eventName;
                throw LuaException(errorMsg.c_str());
            }
            obj.actionToKey[actionInfo->value] = std::move(event);
            return 0;
        });
        return 1;
    }
    return 0;
}
