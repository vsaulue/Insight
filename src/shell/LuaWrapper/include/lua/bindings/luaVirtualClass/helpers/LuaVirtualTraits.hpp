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

#ifndef LUAVIRTUALTRAITS_HPP
#define LUAVIRTUALTRAITS_HPP

#include <memory>
#include <type_traits>

#include "lua/types/LuaTable.hpp"

namespace LuaTraitsImpl {
    /** SFINAE implementation of LuaVirtualTraits::has_getFromTable_T. */
    template<typename T, typename Enable=void>
    struct has_getFromTable_T : std::false_type {};

    template<typename T>
    struct has_getFromTable_T<T, typename std::enable_if<std::is_same<T,decltype(T::luaGetFromTable(std::declval<LuaTable&>()))>::value>::type> : std::true_type {};

    /** SFINAE implementation of LuaVirtualTraits::has_getFromTable_ptr_T. */
    template<typename T, typename Enable=void>
    struct has_getFromTable_ptr_T : std::false_type {};

    template<typename T>
    struct has_getFromTable_ptr_T<T, typename std::enable_if<std::is_same<std::unique_ptr<T>,decltype(T::luaGetFromTable(std::declval<LuaTable&>()))>::value>::type> : std::true_type {};
}

/** Defines some traits for C++ types deriving from LuaVirtualClass binded to Lua. */
template<typename T>
class LuaVirtualTraits {
public:
    /** True if T defines <code>static T luaGetFromTable(LuaTable&);</code>. */
    static constexpr bool has_getFromTable_T = LuaTraitsImpl::has_getFromTable_T<T>::value;
    /** True if T defines <code>static std::unique_ptr<T> luaGetFromTable(LuaTable&);</code>. */
    static constexpr bool has_getFromTable_ptr_T = LuaTraitsImpl::has_getFromTable_ptr_T<T>::value;
    /** True if this type is copy constructible or move constructible. */
    static constexpr bool is_copy_or_move_constructible = std::is_copy_constructible<T>::value || std::is_move_constructible<T>::value;
};

#endif /* LUAVIRTUALTRAITS_HPP */
