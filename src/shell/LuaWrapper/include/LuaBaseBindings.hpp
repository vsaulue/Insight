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

#ifndef LUABINDINGCFUNC_HPP
#define LUABINDINGCFUNC_HPP

#include "LuaBinding.hpp"
#include "LuaStateView.hpp"

struct lua_State;

/** See LuaBinding in LuaBinding.hpp. */
template<>
class LuaBinding<int(*)(lua_State*)> {
private:
    /** Type binded by this LuaBinding<>. */
    using func_ptr = int(*)(lua_State*);
public:
    /**
     * Pushes a C function on the LUA stack.
     *
     * @param state Lua state in which the push is done.
     * @param function Function to push on the stack.
     */
    static void push(LuaStateView& state, func_ptr function) {
        state.pushCFunction(function);
    }

    /**
     * Get a C function at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired function pointer, if the object in the stack is of this type.
     */
    static func_ptr get(LuaStateView& state, int stackIndex) {
        return state.getCFunction(stackIndex);
    }
};

/** See LuaBinding in LuaBinding.hpp. */
template<>
class LuaBinding<bool> {
public:
    /**
     * Gets the name of the metatable of this type.
     *
     * @return The name of this C++ type in Lua.
     */
    static const std::string& luaClassName() {
        static const std::string className("bool");
        return className;
    }

    static void push(LuaStateView& state, bool value) {
        state.pushBool(value);
    }

    static bool get(LuaStateView& state, int stackIndex) {
        return state.getBool(stackIndex);
    }
};

#endif /* LUABINDINGCFUNC_HPP */

