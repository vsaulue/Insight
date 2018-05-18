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

#ifndef LUADEFAULTGET_HPP
#define LUADEFAULTGET_HPP

#include <string>
#include <type_traits>

#include "lua/LuaBinding.hpp"
#include "lua/LuaStateView.hpp"
#include "lua/types/LuaTable.hpp"

namespace detail {
    /**
     * Tests if LuaBinding<BoundType> defines a contructor from a Lua table.
     */
    template<typename BoundType, typename Enable=void>
    struct HasTableConstructor {
        static constexpr bool value = false;
    };

    template<typename BoundType>
    struct HasTableConstructor<BoundType, std::void_t<decltype(LuaBinding<BoundType>::getFromTable(std::declval<LuaTable&>()))>> {
        static constexpr bool value = true;
    };
}

/**
 * Provides a default implementation for LuaBinding<BindedType>::get().
 *
 * @tparam BoundType C++ type to bind to Lua.
 */
template<typename BoundType>
class LuaDefaultGet {
public:
    /** True if BoundType is copy constructible.*/
    static constexpr bool is_copy_constructible = std::is_copy_constructible<BoundType>::value;

    /** True if this C++ type can be constructed from a Lua table.*/
    static constexpr bool has_table_constructor = detail::HasTableConstructor<BoundType>::value;

    /**
     * Builds a C++ object from the Lua stack item at the specified index.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return A copy of the object at the given index, if it is of type T.
     */
    static BoundType get(LuaStateView& state, int stackIndex) {
        static_assert(is_copy_constructible || has_table_constructor, "BoundType can't be constructed from the stack.");
        if (state.isTable(stackIndex)) {
            if constexpr (has_table_constructor) {
                LuaTable table = state.get<LuaTable>(stackIndex);
                return LuaBinding<BoundType>::getFromTable(table);
            } else {
                std::string msg = std::string("Type ") + LuaBinding<BoundType>::luaClassName() + " can't be constructed from a Lua table.";
                state.throwArgError(stackIndex, msg);
            }
        }
        if constexpr (is_copy_constructible) {
            return LuaBinding<BoundType>::getRef(state, stackIndex);
        }
    }
};

#endif /* LUADEFAULTGET_HPP */
