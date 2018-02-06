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

#ifndef LUAMETHOD_HPP
#define LUAMETHOD_HPP

#include <type_traits>

#include "LuaBinding.hpp"
#include "LuaBasicBinding.hpp"
#include "LuaStateView.hpp"

/**
 * Implements a method callable in Lua, working on a C++ type.
 *
 * This method will work on any C++ type that can be dereferenced into T (example: T**).
 */
template<class T>
class LuaMethod {
private:
    /**
     * Function implementing the method.
     */
    int(*method)(T&, LuaStateView&);
public:
    /**
     * Creates a new LuaMethod.
     *
     * @param method Implementation of the method.
     */
    LuaMethod(int(*method)(T&, LuaStateView&)) : method(method) {

    }

    /**
     * Lua __call metamethod implementation.
     *
     * @param state State in which this method is called.
     * @return The number of return values pushed on the Lua stack.
     */
    int operator()(LuaStateView& state) {
        T& object = state.dereferenceGet<T>(1);
        return method(object, state);
    }
};

// See LuaBinding.hpp
template<class T>
class LuaBinding<LuaMethod<T>> : public LuaBasicBinding<LuaMethod<T>> {

};

#endif /* LUAMETHOD_HPP */

