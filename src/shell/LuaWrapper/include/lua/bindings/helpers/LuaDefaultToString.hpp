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

#ifndef LUADEFAULTTOSTRING_HPP
#define LUADEFAULTTOSTRING_HPP

#include <type_traits>

#include "lua/LuaBinding.hpp"
#include "lua/LuaStateView.hpp"
#include "lua/types/LuaNativeString.hpp"

/**
 * Default implementation of Lua metamethod __tostring.
 *
 * @tparam BoundType Type for which the metamethod should be defined.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename BoundType, typename Enable=void>
struct LuaDefaultToString {
public:
    /** Flag indicating if a default __tostring method is defined by this object. */
    static constexpr bool hasToStringFunction = false;
};

template<typename BoundType>
struct LuaDefaultToString<BoundType, std::void_t<decltype(LuaBinding<BoundType>::luaToStringImpl)>> {
public:
    /** Flag indicating if a default __tostring method is defined by this object. */
    static constexpr bool hasToStringFunction = true;

    /**
     * Default implementation of Lua metamethod __tostring for BoundType.
     *
     * @param state State calling the __tostring method.
     * @return The result of __tostring method.
     */
    static int luaToString(LuaStateView& state) {
        BoundType& object = state.getRef<BoundType>(1);
        std::string str = LuaBinding<BoundType>::luaToStringImpl(object);
        state.push<LuaNativeString>(str.c_str());
        return 1;
    }
};

#endif /* LUADEFAULTTOSTRING_HPP */
