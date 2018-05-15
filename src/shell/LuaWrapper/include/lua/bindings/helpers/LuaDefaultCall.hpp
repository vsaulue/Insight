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

#ifndef LUADEFAULTCALL_HPP
#define LUADEFAULTCALL_HPP

#include <type_traits>
#include <utility>

#include "lua/LuaStateView.hpp"

/**
 * Generates a default implementation of Lua metamethod __call for a type.
 *
 * @tparam BoundType Type for which the __call method is generated.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename BoundType, typename Enable=void>
class LuaDefaultCall {
public:
    /* Flag indicating if this class defines a __call metamethod. */
    static constexpr bool hasCall = false;
};

/**
 * Tests if LuaBinding<T>::luaCallImpl exists and can be wrapped into a __call metamethod.
 */
template<typename T>
using void_t_if_hasCallImpl = std::void_t<decltype(LuaBinding<T>::luaCallImpl(std::declval<T&>(), std::declval<LuaStateView&>()))>;

// See non-specialized description.
template<typename BoundType>
class LuaDefaultCall<BoundType, void_t_if_hasCallImpl<BoundType>> {
public:
    static constexpr bool hasCall = true;

    /**
     * Implementation of Lua __call metamethod for BoundType.
     *
     * @param state State calling the metamethod.
     * @return The number of return value on the Lua stack.
     */
    static int luaCall(LuaStateView& state) {
        BoundType& object = state.getRef<BoundType>(1);
        state.remove(1);
        return LuaBinding<BoundType>::luaCallImpl(object, state);
    }
};

#endif /* LUADEFAULTCALL_HPP */

