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

#ifndef LUAPOINTERBINDINGS_HPP
#define LUAPOINTERBINDINGS_HPP

#include <type_traits>

#include "LuaBinding.hpp"
#include "LuaDefaultBinding.hpp"
#include "LuaStateView.hpp"

/**
 * Class defining Lua bindings for the C++ type PointedType*.
 *
 * This class should define the static methods of LuaBinding<PointedType*>.
 *
 * @tparam PointedType Type which pointer will be binded in Lua.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename PointedType, typename Enabled=void>
class LuaPointerBinding;

/** 
 * See LuaBinding in LuaBinding.hpp. 
 * 
 * Default template specialization for pointer types.
 */
template <typename PointedType>
class LuaBinding<PointedType*> : public LuaPointerBinding<PointedType> {

};

// Default implementation, for pointers to non-polymorphic types.
template<typename PointedType>
class LuaPointerBinding<PointedType, std::void_t<decltype(LuaBinding<PointedType>::luaClassName)>> : public LuaDefaultBinding<PointedType*> {
public:
    /**
     * Gets the name of the metatable of this type.
     *
     * @return The name of this C++ type in Lua.
     */
    static const std::string& luaClassName() {
        static const std::string className(LuaBinding<PointedType>::luaClassName() + "*");
        return className;
    }
};

#endif /* LUAPOINTERBINDINGS_HPP */

