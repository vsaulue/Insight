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

#ifndef LUADEFAULTINDEX_HPP
#define LUADEFAULTINDEX_HPP

#include <type_traits>

#include "lua/LuaBinding.hpp"
#include "lua/LuaStateView.hpp"

/**
 * Generates a default implementation of Lua metamethod __index for a type.
 *
 * Default case: no metamethod.
 *
 * @tparam BindedType Type for which the __index method is generated.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename BindedType, typename Enable = void>
class LuaDefaultIndex {
public:
    /** Tells if this type defines a useful Lua __index metamethod. */
    static constexpr bool hasIndex = false;

    /** Implementation of Lua __index metamethod. */
    static int luaIndex(LuaStateView& state) {
        return 0;
    }
};

template<typename T>
using void_t_hasIndexImpl = std::void_t<decltype(LuaBinding<T>::luaIndexImpl(std::declval<T&>(), std::string(), std::declval<LuaStateView&>()))>;

/**
 * Generates a default implementation of Lua metamethod __index for a type.
 *
 * Specialization enabled if LuaBinding of BindedType defines the following static function:
 * <code>
 * int luaIndexImpl(BindedType&, const std::string& memberName, LuaStateView& state);
 * </code>
 * In this case, the metamethod __index of the type will be a wrapper of
 * luaIndexImpl.
 *
 * @tparam BindedType Type for which the __index method is generated.
 */
template<typename BindedType>
class LuaDefaultIndex<BindedType, void_t_hasIndexImpl<BindedType>> {
public:
    static constexpr bool hasIndex = true;

    static int luaIndex(LuaStateView& state) {
        BindedType& object = state.getRef<BindedType>(1);
        std::string memberName(state.get<const char*>(2));

        return LuaBinding<BindedType>::luaIndexImpl(object, memberName, state);
    }
};

#endif /* LUADEFAULTINDEX_HPP */

