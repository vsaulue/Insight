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

#ifndef LUAFUNCTION_HPP
#define LUAFUNCTION_HPP

#include <functional>
#include <utility>

#include "LuaBasicBinding.hpp"
#include "LuaBinding.hpp"
#include "LuaStateView.hpp"

/**
 * Wrapper of a std::function that can be called from Lua.
 */
struct LuaFunction {
    /** Wrapped function callable from Lua. */
    std::function<int(LuaStateView&)> function;

    /**
     * Creates a new function callable from Lua.
     *
     * @param functionCtorArgs Parameters passed to std::function constructor.
     * @tparam ArgsType Types of the parameters passed to std::function constructor.
     */
    template<typename... ArgsType>
    LuaFunction(ArgsType&&... functionCtorArgs) : function(std::forward<ArgsType>(functionCtorArgs)...) {

    }

    LuaFunction(const LuaFunction&) = default;
    LuaFunction(LuaFunction&&) = default;
    LuaFunction(LuaFunction&) = default;

    /**
     * Calls the wrapped std::function.
     *
     * @param state Lua state calling this function
     * @return The number of return values pushed on the Lua stack.
     */
    int operator()(LuaStateView& state) {
        return function(state);
    }
};

/** See LuaBinding.hpp */
template<>
class LuaBinding<LuaFunction> : public LuaBasicBinding<LuaFunction> {

};

#endif /* LUAFUNCTION_HPP */
