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

#include <unordered_map>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lua/LuaException.hpp"
#include "lua/LuaStateView.hpp"

LuaStateView::LuaStateView(lua_State* state) : state(state) {}

double LuaStateView::checkNumber(int stackIndex) {
    return luaL_checknumber(state, stackIndex);
}

bool LuaStateView::isNumber(int stackIndex) {
    return lua_isnumber(state, stackIndex) != 0;
}

bool LuaStateView::isNil(int stackIndex) {
    return lua_isnil(state, stackIndex) != 0;
}

void LuaStateView::doFile(const std::string& filename) {
    int err = luaL_dofile(state, filename.c_str());
    if (err != 0) {
        throw LuaException(state);
    }
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

bool LuaStateView::isTable(int stackIndex) {
    return lua_istable(state, stackIndex) != 0;
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

void LuaStateView::setField(int stackIndex) {
    lua_settable(state, stackIndex);
}

void LuaStateView::getField(int stackIndex) {
    lua_gettable(state, stackIndex);
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

bool LuaStateView::next(int stackIndex) {
    return lua_next(state, stackIndex) != 0;
}

LuaStateView::~LuaStateView() {}

void LuaStateView::pop(int n) {
    lua_pop(state, n);
}

void LuaStateView::openLib(Lib lib) {
    static const std::unordered_map<Lib,std::pair<int(*)(lua_State*),const char*>> openFunctions = {
        {Lib::base, {luaopen_base, ""}},
        {Lib::coroutine, {luaopen_coroutine, LUA_COLIBNAME}},
        {Lib::table, {luaopen_table, LUA_TABLIBNAME}},
        {Lib::io, {luaopen_io, LUA_IOLIBNAME}},
        {Lib::os, {luaopen_os, LUA_OSLIBNAME}},
        {Lib::string, {luaopen_string, LUA_STRLIBNAME}},
        {Lib::utf8, {luaopen_utf8, LUA_UTF8LIBNAME}},
        {Lib::bit32, {luaopen_bit32, LUA_BITLIBNAME}},
        {Lib::math, {luaopen_math, LUA_MATHLIBNAME}},
        {Lib::debug, {luaopen_debug, LUA_DBLIBNAME}},
        {Lib::package, {luaopen_package, LUA_LOADLIBNAME}},
    };

    const auto& pair = openFunctions.at(lib);
    luaL_requiref(state, pair.second, pair.first, true);
    pop(1);
}

int LuaStateView::getTop() {
    return lua_gettop(state);
}

int LuaStateView::absIndex(int index) {
    return lua_absindex(state, index);
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

bool LuaStateView::isString(int stackIndex) {
    return lua_isstring(state, stackIndex);
}

void LuaStateView::pushDouble(double value) {
    lua_pushnumber(state, value);
}

double LuaStateView::getDouble(int stackIndex) {
    return luaL_checknumber(state, stackIndex);
}

void LuaStateView::pushNil() {
    lua_pushnil(state);
}

const char* LuaStateView::getTypename(int stackIndex) {
    return luaL_typename(state, stackIndex);
}

void LuaStateView::checkStack(int extra) {
    int success = lua_checkstack(state, extra);
    if (success == 0) {
        throw LuaException("Unable to allocate extra space on the Lua stack.");
    }
}
