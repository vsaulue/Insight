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

#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include <memory>

#include "lua/bindings/helpers/LuaDefaultGet.hpp"
#include "lua/bindings/helpers/LuaDefaultIndex.hpp"
#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/LuaBinding.hpp"

template<typename PointedType, typename Enable>
class LuaSharedPtrBinding : public LuaDefaultBinding<std::shared_ptr<PointedType>> {
public:
    // Ensure PointedType is properly bound.
    static_assert(std::is_same<const std::string&, decltype(LuaBinding<PointedType>::luaClassName())>::value,
                  "Default shared_ptr binding requires the pointed type to be properly bound."
    );

    /**
     * Implementation of Lua __index metamethod (wrapped by LuaDefaultBinding).
     *
     * Defined if PointedType has an index implementation.
     *
     * @param object Object on which the index function is called.
     * @param memberName Name of the field requested in object by Lua.
     * @param state Lua state requesting the field.
     * @return The number of values returned to Lua.
     */
    template<typename T=PointedType>
    static typename std::enable_if<LuaDefaultIndex<T>::hasIndex, int>::type luaIndexImpl(std::shared_ptr<T>& object, const std::string& memberName, LuaStateView& state) {
        PointedType* ptr = object.get();
        if (ptr == nullptr) {
            state.throwArgError(1, "Cannot get field or method from a null pointer.");
        }
        return LuaBinding<PointedType>::luaIndexImpl(*ptr, memberName, state);
    }

    /**
     * Creates a std::shared_ptr directly from a Lua table.
     *
     * Requires T binding to define a table constructor, and T to be copy or move constructible.
     *
     * @param table Table to construct the object from.
     * @return A smart pointer to a C++ object constructed from the given Lua table.
     */
    template<typename T=PointedType>
    static typename std::enable_if<LuaDefaultGet<T>::has_table_constructor, std::shared_ptr<T>>::type getFromTable(LuaTable& table) {
        T value = LuaBinding<T>::getFromTable(table);
        return std::make_shared<T>(value);
    }
};

#endif /* SHARED_PTR_HPP */
