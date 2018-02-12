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

#include "lua/LuaBinding.hpp"
#include "lua/LuaDereferencer.hpp"
#include "lua/LuaStateView.hpp"

#include "lua/bindings/helpers/LuaDefaultClassName.hpp"
#include "lua/bindings/helpers/LuaWrapFunction.hpp"
#include "lua/bindings/LuaDefaultBinding.hpp"

#include "lua/types/LuaDereferenceGetter.hpp"
#include "lua/types/LuaUpcaster.hpp"

// Default implementation, for pointers to non-polymorphic types.
template<typename PointedType, typename Enabled>
class LuaPointerBinding : public LuaDefaultBinding<PointedType*> {
public:
    // Ensure PointedType is properly binded.
    static_assert(std::is_same<const std::string&, decltype(LuaBinding<PointedType>::luaClassName())>::value,
                  "Default pointer bindings requires the pointed type to be properly binded."
    );

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

