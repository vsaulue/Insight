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
#include "LuaDefaultClassName.hpp"
#include "LuaDefaultBinding.hpp"
#include "LuaDereferenceGetter.hpp"
#include "LuaDereferencer.hpp"
#include "LuaBaseBindings.hpp"
#include "LuaStateView.hpp"
#include "LuaUpcaster.hpp"
#include "LuaWrapFunction.hpp"

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
template<typename PointedType, typename Enabled>
class LuaPointerBinding : public LuaDefaultBinding<PointedType*> {
public:
    /**
     * Implementation of Lua __index metamethod (wrapped by LuaDefaultBinding).
     *
     * @param object Object on which the index function is called.
     * @param memberName Name of the field requested in object by Lua.
     * @param state Lua state requesting the field.
     * @return The number of values returned to Lua.
     */
    template<typename T=PointedType>
    static typename std::enable_if<LuaDefaultIndex<T>::hasIndex, int>::type luaIndexImpl(PointedType* object, const std::string& memberName, LuaStateView& state) {
        if (object == nullptr) {
            state.throwArgError(1, "Cannot get field or method from a null pointer.");
        }
        return LuaBinding<PointedType>::luaIndexImpl(*object, memberName, state);
    }
};

#endif /* LUAPOINTERBINDINGS_HPP */

