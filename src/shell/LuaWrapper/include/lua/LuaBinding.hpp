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

/** @file */

#ifndef LUABINDING_HPP
#define LUABINDING_HPP

/**
 * Template class used to bind C++ object to/from Lua userdata.
 *
 * The class LuaStateView visits (UML pattern) this template to push/get C++ objects to/from the Lua stack.
 *
 * Specializations should define the following public static methods:
 * <ul>
 *   <li>
 *     <code>
 *       template<typename ConstructorArgs...> void push(LuaStateView& state, ConstructorArgs... args);
 *     </code>
 *     <p>
 *       Pushes and initialize a new object on the Lua stack. For basic types (boolean, strings, ...),
 *       this function might use built-in types from Lua. For more complex type, a new C++ object of
 *       type BindedType will be created, wrapped into a Userdata Lua type, and assigned a metatable.
 *     </p>
 *   </li>
 *   <li>
 *     <code>
 *       BindedType get(LuaStateView& state, int stackIndex)
 *     </code>
 *     <p>
 *       Checks the element at the given index is compatible with the given C++ type. If the check succeeded,
 *       it will return a copy the C++ object.
 *     </p>
 *   </li>
 *   <li>
 *     <code>
 *       BindedType getRef(LuaStateView& state, int stackIndex) // optional
 *     </code>
 *     <p>
 *       Checks the element at the given index is compatible with the given C++ type. If the check succeeded,
 *       it will return a reference to the C++ object stored inside Lua.
 *
 *       This function might not be defined for basic types of Lua (like bool, double, C functions, ...).
 *     </p>
 *   </li>
 * </ul>
 *
 * @tparam BindedType type that will be binded in Lua.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template <typename BindedType, typename Enable = void> class LuaBinding;

#endif /* LUABINDING_HPP */

