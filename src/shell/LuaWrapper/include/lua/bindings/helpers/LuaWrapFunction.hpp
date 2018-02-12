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

#ifndef LUAWRAPFUNCTION_HPP
#define LUAWRAPFUNCTION_HPP

#include "lua/LuaStateView.hpp"

struct lua_State;

/**
 * Wraps a function int(*)(LuaStateView&) into a function into a native Lua C function.
 *
 * Provides a uniform way to handle exceptions.
 *
 * @tparam wrappedFunction C++ function to wrap.
 *
 * @param[in] cState The Lua state calling this function.
 * @return The number of return value of this function on the Lua stack.
 */
template<int(*wrappedFunction)(LuaStateView&)>
int luaWrapFunction(lua_State* cState) {
    LuaStateView state(cState);
    int result = 0;
    try {
        result = wrappedFunction(state);
    } catch (const std::exception& e) {
        std::string msg("C++ exception: ");
        msg += e.what();
        state.throwError(msg);
    }
    return result;
}

#endif /* LUAWRAPFUNCTION_HPP */

