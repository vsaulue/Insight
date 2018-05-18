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
 * Metaprogramming object used to check if a C++ type bound in Lua is a stack alias.
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
 * <p>
 *   The LuaBinding of such an object must define a function to set the ownership inside the C++ object:
 *   <code>static void setStackOwnership(BoundType& object, bool value);</code>
 * </p>
 */
template<typename BoundType, typename Enable=void>
struct LuaIsStackAlias {
public:
    /** False if the item on the Lua stack can be popped after the C++ object was constructed. True otherwise. */
    static constexpr bool value = false;
};

// See non-specialized description.
template<typename BoundType>
struct LuaIsStackAlias<BoundType, std::void_t<decltype(LuaBinding<BoundType>::isStackAlias)>> {
public:
    static constexpr bool value = LuaBinding<BoundType>::isStackAlias;
private:
    // SFINAE implementation of has_setStackOwnership.
    template<typename T, typename Enable=void>
    struct has_setStackOwnershipImpl : std::false_type {};

    template<typename T>
    struct has_setStackOwnershipImpl<T,decltype(LuaBinding<T>::setStackOwnership(std::declval<T&>(), true))> : std::true_type {};

    /** True if the LuaBinding<BoundType>::setStackOwnership(T&,bool) is defined. */
    static constexpr bool has_setStackOwnership = has_setStackOwnershipImpl<BoundType>::value;

    static_assert(!(value && !has_setStackOwnership), "The binding of a stack alias must define: static void setStackOwnership(T&,bool);");
};

#endif /* LUAISSTACKALIAS_HPP */
