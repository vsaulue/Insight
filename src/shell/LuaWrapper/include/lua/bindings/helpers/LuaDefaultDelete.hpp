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

#ifndef LUADEFAULTDELETE_HPP
#define LUADEFAULTDELETE_HPP

#include <type_traits>

#include "lua/LuaStateView.hpp"

/**
 * Generates a default implementation of Lua metamethod __gc for a type.
 *
 * @tparam BindedType Type for which the __gc method is generated.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename BindedType, typename Enable=void>
class LuaDefaultDelete {
public:
    /**
     * Implementation of Lua metamethod "__gc" for this type.
     *
     * @param state State calling this function.
     *
     * @return The number of return values of this Lua function.
     */
    static int luaDelete(LuaStateView& state) {
        BindedType& object = state.getRef<BindedType>(1);
        object.~BindedType();
        return 0;
    }

    /** Indicates if this template defines a useful __gc method for Lua. */
    static constexpr bool hasDeletor = true;
};

template<typename BindedType>
class LuaDefaultDelete<BindedType, typename std::enable_if<std::is_trivially_destructible<BindedType>::value>::type> {
public:
    /**
     * Implementation of Lua metamethod "__gc" for this type.
     *
     * @param state State calling this function.
     *
     * @return The number of return values of this Lua function.
     */
    static int luaDelete(LuaStateView& state) {
        return 0;
    }

    /** Indicates if this template defines a useful __gc method for Lua. */
    static constexpr bool hasDeletor = false;
};

#endif /* LUADEFAULTDELETE_HPP */

