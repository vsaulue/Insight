/* This file is part of Insight.
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

#include "LuaException.hpp"

#include "lua.hpp"

LuaException::LuaException(lua_State* state) : state(state) {
}

const char* LuaException::what() const noexcept {
    const char *res = lua_tostring(state, -1);
    if (res == nullptr) {
        res = "Unknown LUA error.";
    }
    return res;
}

LuaException::~LuaException() {
    lua_pop(state, 1);
}
