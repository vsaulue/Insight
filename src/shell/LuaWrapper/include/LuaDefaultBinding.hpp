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

#ifndef LUADEFAULTBINDING_HPP
#define LUADEFAULTBINDING_HPP

#include "LuaBasicBinding.hpp"

/**
 * Provides default implementations of some functions of LuaBinding.
 *
 * This will define the following functions for Luabinding:
 * - push
 * - getRef
 * - get (if BindedType is copy constructible).
 *
 * For reference types, it will add dereferencing functions in the metatable. This enables LuaMethod<LuaBasetype<BindedType>>
 * to work on an object of type BindedType.
 *
 * LuaBinding<BindedType> can be derived from this object. In this case, the
 * LuaBinding<BindedType> specialization must provide the following static method:
 * <code>
 *     static const std::string& luaClassName();
 *     // Gets the name of the Lua class wrapping the C++ type BindedType. Must be unique.
 * </code>
 *
 * Optional static method:
 * <code>
 *     static int luaIndex(BindedType& object, const std::string& memberName, LuaStateView& state);
 *     // Gets the field/method of object named memberName (see lua metamethod __index).
 * </code>
 *
 * @tparam BindedType C++ type that will be binded into a Lua type.
 */
template<typename BindedType>
class LuaDefaultBinding : public LuaBasicBinding<BindedType> {

};

#endif /* LUADEFAULTBINDING_HPP */

