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

#include <memory>

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
 *       type BoundType will be created, wrapped into a Userdata Lua type, and assigned a metatable.
 *     </p>
 *   </li>
 *   <li>
 *     <code>
 *       BoundType get(LuaStateView& state, int stackIndex)
 *     </code>
 *     <p>
 *       Checks the element at the given index is compatible with the given C++ type. If the check succeeded,
 *       it will return a copy the C++ object.
 *     </p>
 *   </li>
 *   <li>
 *     <code>
 *       BoundType getRef(LuaStateView& state, int stackIndex) // optional
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
 * Specializations can define the following static constexpr members:
 * <ul>
 *   <li>
 *     <code>
 *       bool isStackAlias; // optional: default = false
 *     </code>
 *     <p>
 *       Indicates that the BoundType instance returned by get() is just an alias of a stack item.
 *       This value is used in LuaTable to handle the lifespan of objects read from a Lua table.
 *     </p>
 *   </li>
 * </ul>
 *
 * @tparam BoundType type that will be bound in Lua.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template <typename BoundType, typename Enable = void> class LuaBinding;

/**
 * Class defining Lua bindings for the C++ type PointedType*.
 *
 * This class should define the static methods of LuaBinding<PointedType*>.
 *
 * @tparam PointedType Type which pointer will be bound in Lua.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename PointedType, typename Enabled=void>
class LuaPointerBinding;

/**
 * Default template specialization for pointer types.
 */
template <typename PointedType>
class LuaBinding<PointedType*> : public LuaPointerBinding<PointedType> {

};

/**
 * Class defining Lua bindings for the C++ type std::shared_ptr<PointedType>.
 *
 * This class should define the static methods of LuaBinding<std::shared_ptr<PointedType>>.
 *
 * @tparam PointedType Type which pointer will be bound in Lua.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename PointedType, typename Enabled=void>
class LuaSharedPtrBinding;

/**
 * Default template specialization for std::shared_ptr types.
 */
template<typename PointedType>
class LuaBinding<std::shared_ptr<PointedType>> : public LuaSharedPtrBinding<PointedType> {

};

#endif /* LUABINDING_HPP */

