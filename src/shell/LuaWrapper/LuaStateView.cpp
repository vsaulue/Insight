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

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lua/LuaException.hpp"
#include "lua/LuaStateView.hpp"

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

void LuaStateView::pushBool(bool value) {
    lua_pushboolean(state, value);
}

bool LuaStateView::getBool(int stackIndex) {
    return lua_toboolean(state, stackIndex);
}

void LuaStateView::pushCFunction(int(*function)(lua_State*)) {
    lua_pushcfunction(state, function);
}

LuaStateView::CFunction LuaStateView::getCFunction(int stackIndex) {
    int (*result)(lua_State*) = nullptr;
    if (lua_iscfunction(state, stackIndex)) {
        result = lua_tocfunction(state, stackIndex);
    } else {
        std::string msg("expected C Function int(*)(lua_State), got");
        msg += luaL_typename(state, stackIndex);
        luaL_argerror(state, stackIndex, msg.c_str());
    }
    return result;
}

void LuaStateView::remove(int stackIndex) {
    lua_remove(state, 1);
}

void LuaStateView::throwError(const std::string& msg) {
    luaL_error(state, msg.c_str());
}

void* LuaStateView::checkUserData(int stackIndex) {
    void* result = nullptr;
    if (lua_isuserdata(state, stackIndex)) {
        result = lua_touserdata(state, stackIndex);
    } else {
        std::string msg("expected Userdata, got");
        msg += luaL_typename(state, stackIndex);
        luaL_argerror(state, stackIndex, msg.c_str());
    }
    return result;
}

bool LuaStateView::pushMetafield(int stackIndex, const std::string& fieldName) {
    int res = luaL_getmetafield(state, stackIndex, fieldName.c_str());
    return (res != 0);
}

void LuaStateView::throwArgError(int stackIndex, const std::string& msg) {
    luaL_argerror(state, stackIndex, msg.c_str());
}

void LuaStateView::pushString(const char* value) {
    lua_pushstring(state, value);
}

const char* LuaStateView::getString(int stackIndex) {
    return luaL_checkstring(state, stackIndex);
}

void LuaStateView::pushDouble(double value) {
    lua_pushnumber(state, value);
}

double LuaStateView::getDouble(int stackIndex) {
    return luaL_checknumber(state, stackIndex);
}


const char* LuaStateView::getTypename(int stackIndex) {
    return luaL_typename(state, stackIndex);
}
