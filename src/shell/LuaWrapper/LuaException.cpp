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

#include "lua/LuaException.hpp"

#include "lua.hpp"

LuaException::LuaException(lua_State* state) {
    const char* msg = lua_tostring(state, -1);
    if(msg == nullptr) {
        errorMsg = "Unknown LUA error.";
    } else {
        errorMsg = msg;
        lua_pop(state, 1);
    }
}

LuaException::LuaException(const char* msg) : errorMsg(msg) {

}

const char* LuaException::what() const noexcept {
    return errorMsg.c_str();
}
