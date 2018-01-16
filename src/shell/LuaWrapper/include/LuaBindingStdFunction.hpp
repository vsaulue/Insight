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

#ifndef LUABINDINGSTDFUNCTION_HPP
#define LUABINDINGSTDFUNCTION_HPP

#include <functional>

#include "LuaBinding.hpp"
#include "LuaBindingFunc.hpp"
#include "LuaStateView.hpp"
#include "LuaWrapFunction.hpp"

/** See LuaBinding in LuaBinding.hpp. */
template<>
class LuaBinding<std::function<int(LuaStateView&)>> {
private:
    using FuncType = std::function<int(LuaStateView&)>;

    /**
     * Get the name of the metatable of this table.
     *
     * @return The name of this C++ type in Lua.
     */
    static const std::string& luaClassName() {
        static const std::string className("LuaStdFunction");
        return className;
    }

    /**
     * Implementation of Lua metamethod "__call" for this type.
     *
     * @param state State calling this function.
     *
     * @return The number of return values of this Lua function.
     */
    static int luaCall(LuaStateView& state) {
        FuncType& function(state.getRef<FuncType>(1));
        state.remove(1);
        return function(state);
    }

    /**
     * Implementation of Lua metamethod "__gc" for this type.
     *
     * @param state State calling this function.
     *
     * @return The number of return values of this Lua function.
     */
    static int luaDelete(LuaStateView& state) {
        FuncType& function(state.getRef<FuncType>(1));
        function.~FuncType();
        return 0;
    }

    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state) {
        bool newTable = state.newMetatable(luaClassName());
        if (newTable) {
            state.push<int(*)(lua_State*)>(luaWrapFunction<luaCall>);
            state.setField(-2, "__call");
            state.push<int(*)(lua_State*)>(luaWrapFunction<luaDelete>);
            state.setField(-2,"__gc");
        }
    }
public:
    /**
     * Pushes a std::function on the LUA stack.
     *
     * @param state Lua state in which the push is done.
     * @param function Function to push on the stack.
     */
    static void push(LuaStateView& state, const std::function<int(LuaStateView&)>& function) {
        state.newObject<FuncType>(function);
        pushMetatable(state);
        state.setMetatable(-2);
    }

    /**
     * Get a reference to a std::function at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired function object, if the object in the stack is of this type.
     */
    static std::function<int(LuaStateView&)>& getRef(LuaStateView& state, int stackIndex) {
        FuncType* result = state.checkUserData<FuncType>(stackIndex, luaClassName());
        return *result;
    }

    /**
     * Get a std::function at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired function object, if the object in the stack is of this type.
     */
    static std::function<int(LuaStateView&)> get(LuaStateView& state, int stackIndex) {
        return getRef(state, stackIndex);
    }
};

#endif /* LUABINDINGSTDFUNCTION_HPP */

