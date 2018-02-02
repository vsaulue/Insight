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

#ifndef LUADEFAULTDEREFERENCER_HPP
#define LUADEFAULTDEREFERENCER_HPP

#include "LuaException.hpp"
#include "LuaStateView.hpp"

/**
 * Creates a default dereferencing function for the specified type.
 *
 * @tparam BindedType Type to dereference.
 */
template<typename BindedType>
class LuaDefaultDereferencer {
public:
    static constexpr bool hasDereferencer = true;

    using basetype = BindedType;

    static basetype& dereferenceGetter(LuaStateView& state, int stackIndex) {
        return state.getRef<basetype>(stackIndex);
    }

    static basetype& dereference(basetype& ref) {
        return ref;
    }
};

template<typename PointedType>
class LuaDefaultDereferencer<PointedType*> {
public:
    static constexpr bool hasDereferencer = true;

    using basetype = typename LuaDefaultDereferencer<PointedType>::basetype;

    static basetype& dereferenceGetter(LuaStateView& state, int stackIndex) {
        PointedType* ptr = state.get<PointedType*>(stackIndex);
        return dereference(ptr);
    }

    static basetype& dereference(PointedType* ptr) {
        if (ptr == nullptr) {
            throw LuaException("Attempt to dereference nullptr.");
        }
        return LuaDefaultDereferencer<PointedType>::dereference(*ptr);
    }
};

template<typename T>
using LuaBasetype = typename LuaDefaultDereferencer<T>::basetype;

#endif /* LUADEFAULTDEREFERENCER_HPP */

