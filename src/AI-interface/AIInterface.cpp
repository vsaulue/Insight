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

#include "AIInterface.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "lua/types/LuaTable.hpp"

AIInterface::AIInterface() = default;

AIInterface::~AIInterface() = default;

int AIInterface::luaIndex(const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="senses") {
        LuaTable result(state, false);
        for (auto& pair : senses) {
            result.set<LuaNativeString,SenseSignal*>(pair.first.c_str(), pair.second);
        }
    } else if (memberName=="actions") {
        LuaTable result(state, false);
        for (auto& pair : actions) {
            result.set<LuaNativeString,ActionSignal*>(pair.first.c_str(), pair.second);
        }
    } else {
        result = 0;
    }
    return result;
}
