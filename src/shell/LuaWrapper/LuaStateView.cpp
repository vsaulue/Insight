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

void* LuaStateView::checkUData(int stackIndex, const std::string& metatableName) {
    return luaL_checkudata(state, stackIndex, metatableName.c_str());
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

void LuaStateView::open_base() {
    luaopen_base(state);
}

int LuaStateView::getTop() {
    return lua_gettop(state);
}

void LuaStateView::pushCFunction(int(*function)(lua_State*)) {
    lua_pushcfunction(state, function);
}

LuaStateView::CFunction LuaStateView::getCFunction(int stackIndex) {
    int (*result)(lua_State*) = nullptr;
    if (lua_iscfunction(state, stackIndex)) {
        result = lua_tocfunction(state, stackIndex);
    } else {
        luaL_error(state, "Error: expected a C Function at index %d\n", stackIndex);
    }
    return result;
}

void LuaStateView::remove(int stackIndex) {
    lua_remove(state, 1);
}
