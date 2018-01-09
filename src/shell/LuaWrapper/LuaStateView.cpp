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
#include "LuaStateView.hpp"

#include "lua.hpp"

LuaStateView::LuaStateView(lua_State* state) : state(state) {}

double LuaStateView::checkNumber(int stackIndex) {
    return luaL_checknumber(state, stackIndex);
}

void LuaStateView::doString(const std::string& code) {
    int err = luaL_dostring(state, code.c_str());
    if (err != 0) {
        throw LuaException(state);
    }
}

bool LuaStateView::newMetatable(const std::string& name) {
    int ret = luaL_newmetatable(state, name.c_str());
    return ret == 1;
}

void LuaStateView::newTable() {
    lua_newtable(state);
}

void* LuaStateView::newUserData(std::size_t size) {
    return lua_newuserdata(state, size);
}

void LuaStateView::pushString(const std::string& str) {
    lua_pushstring(state, str.c_str());
}

void LuaStateView::pushValue(int stackIndex) {
    lua_pushvalue(state, stackIndex);
}

void LuaStateView::setField(int stackIndex, const std::string& fieldName) {
    lua_setfield(state, stackIndex, fieldName.c_str());
}

void LuaStateView::setFuncs(const luaL_Reg* functions, int nup) {
    luaL_setfuncs(state, functions, nup);
}

void LuaStateView::setGlobal(const std::string& name) {
    lua_setglobal(state, name.c_str());
}

void LuaStateView::setMetatable(int stackIndex) {
    lua_setmetatable(state, stackIndex);
}

void LuaStateView::setTable(int stackIndex) {
    lua_settable(state, stackIndex);
}

LuaStateView::~LuaStateView() {}

void LuaStateView::pop(int n) {
    lua_pop(state, n);
}
