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
 * @tparam BindedType Type for which the __call method is generated.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename BindedType, typename Enable=void>
class LuaDefaultCall {
public:
    static constexpr bool hasCall = false;

    static int luaCall(LuaStateView& state) {
        return 0;
    }
};

template<typename BindedType>
class LuaDefaultCall<BindedType, typename std::enable_if<std::is_same<int,decltype(std::declval<BindedType>()(std::declval<LuaStateView&>()))>::value>::type> {
public:
    static constexpr bool hasCall = true;

    static int luaCall(LuaStateView& state) {
        BindedType& object = state.getRef<BindedType>(1);
        state.remove(1);
        return object(state);
    }
};

#endif /* LUADEFAULTCALL_HPP */

