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
        ActionInfo(Action::CameraForward, "CameraForward"),
        ActionInfo(Action::CameraBackward, "CameraBackward"),
        ActionInfo(Action::CameraLeft, "CameraLeft"),
        ActionInfo(Action::CameraRight, "CameraRight"),
        ActionInfo(Action::CameraUp, "CameraUp"),
        ActionInfo(Action::CameraDown, "CameraDown"),
        ActionInfo(Action::ToggleMouseLock, "ToggleMouseLock"),
        ActionInfo(Action::CameraTurnLeft, "CameraTurnLeft"),
        ActionInfo(Action::CameraTurnRight, "CameraTurnRight"),
        ActionInfo(Action::CameraTurnUp, "CameraTurnUp"),
        ActionInfo(Action::CameraTurnDown, "CameraTurnDown")
    };
    return values;
}


ActionInfo::ActionInfo(Action action, const char* name) :
    name(name),
    value(action)
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
