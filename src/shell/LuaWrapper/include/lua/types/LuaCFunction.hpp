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

#ifndef LUACFUNCTION_HPP
#define LUACFUNCTION_HPP

#include "lua/bindings/helpers/LuaDefaultClassName.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/LuaStateView.hpp"

struct lua_State;

/**
 * Wrapper of a Lua native C function.
 *
 * This C++ type is directly binded as a Lua native type (not userdata).
 */
struct LuaCFunction {
    /**
     * Creates a new Lua native function.
     *
     * @param implementation Pointer to the implementation of the function.
     */
    LuaCFunction(int(*implementation)(lua_State*)) : function(implementation) {

    }

    /** Implementation of the Lua native function. */
    int(*function)(lua_State*);
};

/** See LuaBinding in LuaBinding.hpp. */
template<>
class LuaBinding<LuaCFunction> {
public:
    /**
     * Pushes a C function on the LUA stack.
     *
     * @param state Lua state in which the push is done.
     * @param function Function to push on the stack.
     */
    static void push(LuaStateView& state, LuaCFunction function) {
        state.pushCFunction(function.function);
    }

    /**
     * Get a C function at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired function pointer, if the object in the stack is of this type.
     */
    static LuaCFunction get(LuaStateView& state, int stackIndex) {
        return state.getCFunction(stackIndex);
    }
};

#endif /* LUACFUNCTION_HPP */
