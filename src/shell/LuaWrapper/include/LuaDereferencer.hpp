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

#ifndef LUADEREFERENCER_HPP
#define LUADEREFERENCER_HPP

#include <type_traits>

#include "LuaException.hpp"
#include "LuaStateView.hpp"

/**
 * Creates a default dereferencing function for the specified type.
 *
 * The default template implements a no-op dereferencer: the basetype of BindedType
 * is BindedType.
 *
 * Any specialization should contain the following public members:
 * <li>
 *   <ul>
 *     <code>typedef basetype = ...;</code><br>
 *     Type returned by this dereferencer.
 *   </ul>
 *   <ul>
 *     <code>static basetype& dereferenceGetter(LuaStateView& state, int stackIndex);</code><br>
 *     Function reading a value of type BindedType in the Lua stack, and returning it dereferenced into a basetype.
 *   </ul>
 *   <ul>
 *     <code>static basetype& dereference(BindedType& ref); // or BindedType argument.</code><br>
 *     Function dereferencing a BindedType into a basetype&.
 *   </ul>
 * </li>
 *
 * @tparam BindedType Type to dereference.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename BindedType, typename Enable=void>
class LuaDereferencer {
public:
    /** Type returned by the dereferencer implemented in this class. */
    using basetype = BindedType;

    /**
     * Reads a value of type BindedType in the Lua stack, and dereference it into basetype.
     *
     * @param state State in which the object of type BindedType is stored.
     * @param stackIndex Index of the element in the stack.
     * @return The element dereferenced into BindedType.
     */
    static basetype& dereferenceGetter(LuaStateView& state, int stackIndex) {
        return state.getRef<basetype>(stackIndex);
    }

    /**
     * Dereference BindedType& into basetype&.
     *
     * @param ref Reference into basetype
     * @return
     */
    static basetype& dereference(BindedType& ref) {
        return ref;
    }
};

template<typename PointedType>
class LuaDereferencer<PointedType*> {
public:
    using basetype = typename LuaDereferencer<PointedType>::basetype;

    static basetype& dereferenceGetter(LuaStateView& state, int stackIndex) {
        PointedType* ptr = state.get<PointedType*>(stackIndex);
        return dereference(ptr);
    }

    static basetype& dereference(PointedType* ptr) {
        if (ptr == nullptr) {
            throw LuaException("Attempt to dereference nullptr.");
        }
        return LuaDereferencer<PointedType>::dereference(*ptr);
    }
};

template<typename T>
using LuaBasetype = typename LuaDereferencer<T>::basetype;

#endif /* LUADEREFERENCER_HPP */

