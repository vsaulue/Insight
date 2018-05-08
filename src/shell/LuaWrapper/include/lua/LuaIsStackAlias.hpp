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

#ifndef LUAISSTACKALIAS_HPP
#define LUAISSTACKALIAS_HPP

#include "lua/LuaBinding.hpp"

#include <type_traits>

/**
 * Metaprogramming object used to check if a C++ type binded in Lua is a stack alias.
 *
 * <p>
 *   A C++ type is a stack alias if it requires an item to remain on the stack during its lifespan.
 * </p>
 * <p>
 *   Examples:
 *   <ul>
 *     <li>Stack alias: LuaTable</li>
 *     <li>Non stack alias: LuaNativeString</li>
 *   </ul>
 * </p>
 */
template<typename BindedType, typename Enable=void>
struct LuaIsStackAlias {
public:
    /** False if the item on the Lua stack can be popped after the C++ object was constructed. True otherwise. */
    static constexpr bool value = false;
};

// See non-specialized description.
template<typename BindedType>
struct LuaIsStackAlias<BindedType, std::void_t<decltype(LuaBinding<BindedType>::isStackAlias)>> {
public:
    static constexpr bool value = LuaBinding<BindedType>::isStackAlias;
};

#endif /* LUAISSTACKALIAS_HPP */
