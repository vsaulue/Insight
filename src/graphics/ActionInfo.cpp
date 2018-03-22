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

#include "ActionInfo.hpp"

const std::unordered_set<ActionInfo>& ActionInfo::list() {
    static const std::unordered_set<ActionInfo> values = {
        ActionInfo(Action::CameraForward, "CameraForward", true),
        ActionInfo(Action::CameraBackward, "CameraBackward", true),
        ActionInfo(Action::CameraLeft, "CameraLeft", true),
        ActionInfo(Action::CameraRight, "CameraRight", true),
        ActionInfo(Action::CameraUp, "CameraUp", true),
        ActionInfo(Action::CameraDown, "CameraDown", true),
        ActionInfo(Action::ToggleMouseLock, "ToggleMouseLock", false),
        ActionInfo(Action::CameraTurnLeft, "CameraTurnLeft", true),
        ActionInfo(Action::CameraTurnRight, "CameraTurnRight", true),
        ActionInfo(Action::CameraTurnUp, "CameraTurnUp", true),
        ActionInfo(Action::CameraTurnDown, "CameraTurnDown", true),
    };
    return values;
}


ActionInfo::ActionInfo(Action action, const char* name, bool persistent) :
    name(name),
    value(action),
    persistent(persistent)
{

}

const ActionInfo* ActionInfo::byName(const std::string& name) {
    auto& values = list();
    auto it = std::find_if(values.begin(), values.end(), [&name](const ActionInfo& info) -> bool { return info.name == name; });
    ActionInfo* result = nullptr;
    if (it != values.end()) {
        return &(*it);
    }
    return result;
}
