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
#include <map>

#include "InputSettings.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/LuaException.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaNativeString.hpp"

InputSettings::InputSettings() {
    using ST = SettingsType;
    values[ST::CameraRotationSpeed] = 30.0f;
    values[ST::CameraTranslationSpeed] = 5.0f;
}

float InputSettings::get(SettingsType settings) const {
    return values.at(settings);
}

const std::map<InputSettings::SettingsType, std::string>& InputSettings::list() {
    using ST = SettingsType;
    static const std::map<SettingsType, std::string>& result = {
        {ST::CameraTranslationSpeed, "CameraTranslationSpeed"},
        {ST::CameraRotationSpeed, "CameraRotationSpeed"},
    };
    return result;
}

static InputSettings::SettingsType luaSettingsByName(LuaStateView& state, int stackIndex) {
    std::string settingsName(state.get<LuaNativeString>(stackIndex));
    auto& map = InputSettings::list();
    auto it = std::find_if(map.begin(), map.end(), [&settingsName](auto& pair) -> bool { return pair.second == settingsName; });
    if (it == map.end()) {
        std::string errorMsg = std::string("Invalid settings name: ") + settingsName;
        throw LuaException(errorMsg.c_str());
    }
    return it->first;
}

int InputSettings::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<InputSettings>;
    if (memberName == "list") {
        state.push<Method>([](InputSettings& obj, LuaStateView& state) -> int {
            auto& map = list();
            int result = map.size();
            state.checkStack(result);
            for (auto& pair : map) {
                state.push<LuaNativeString>(pair.second.c_str());
            }
            return result;
        });
        return 1;
    } else if (memberName == "set") {
        state.push<Method>([](InputSettings& obj, LuaStateView& state) -> int {
            SettingsType settings = luaSettingsByName(state, 2);
            float value = state.get<float>(3);
            obj.values[settings] = value;
            return 0;
        });
        return 1;
    } else if (memberName == "get") {
        state.push<Method>([](InputSettings& obj, LuaStateView& state) -> int {
            SettingsType settings = luaSettingsByName(state, 2);
            state.push<float>(obj.values[settings]);
            return 1;
        });
        return 1;
    }
    return 0;
}
