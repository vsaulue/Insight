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

#ifndef LUABINDINGFUNC_HPP
#define LUABINDINGFUNC_HPP

#include <exception>

#include "LuaBinding.hpp"
#include "LuaBindingCFunc.hpp"
#include "LuaStateView.hpp"

/** See LuaBinding in LuaBinding.hpp. */
template<>
class LuaBinding<int(*)(LuaStateView&)> {
private:
    /** Type binded by this LuaBinding<>. */
    using func_ptr = int(*)(LuaStateView&);

    /**
     * Get the name of the metatable of this table.
     *
     * @return The name of this C++ type in Lua.
     */
    static const std::string& luaClassName() {
        static const std::string className("LuaFunction");
        return className;
    }

    /**
     * Implementation of Lua metamethod "__call" for this type.
     *
     * @param cState State calling this function.
     *
     * @return The number of return values of this Lua function.
     */
    static int luaCall(lua_State *cState) {
        LuaStateView state(cState);
        int result = 0;
        try {
            func_ptr function = state.get<func_ptr>(1);
            state.remove(1);
            result = function(state);
        } catch (const std::exception& e) {
            std::string msg("C++ exception: ");
            msg += e.what();
            state.throwError(msg);
        }
        return result;
    }

    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state) {
        bool newTable = state.newMetatable(luaClassName());
        if (newTable) {
            state.push<int(*)(lua_State*)>(luaCall);
            state.setField(-2, "__call");
        }
    }
public:
    /**
     * Pushes a C function on the LUA stack.
     *
     * @param state Lua state in which the push is done.
     * @param function Function to push on the stack.
     */
    static void push(LuaStateView& state, func_ptr function) {
        state.newObject<func_ptr>(function);
        pushMetatable(state);
        state.setMetatable(-2);
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
        func_ptr* result = state.checkUserData<func_ptr>(stackIndex, luaClassName());
        return *result;
    }
};

#endif /* LUABINDINGFUNC_HPP */

