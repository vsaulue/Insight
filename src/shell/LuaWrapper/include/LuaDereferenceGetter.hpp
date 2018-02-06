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

#ifndef LUADEREFERENCEGETTER_HPP
#define LUADEREFERENCEGETTER_HPP

#include <type_traits>
#include <typeinfo>

#include "LuaBinding.hpp"
#include "LuaBasicBinding.hpp"
#include "LuaVirtualClass.hpp"

/**
 * Function wrapper used to get a reference to the base type at the specified index.
 *
 * A LuaDereferenceGetter<Basetype> is stored in Lua metatable of any C++ type that
 * can be dereferenced into a Basetype.
 *
 * @tparam Basetype Type returned by this function (after dereferencing the value in the stack).
 */
template<typename Basetype>
class LuaDereferenceGetter {
private:
    /** Function retrieving the object from the stack, and dereferencing in into Basetype. */
    Basetype&(*get)(LuaStateView&, int);
public:
    /**
     * Constructs a LuaDereferenceGetter initialized with nullptr.
     */
    LuaDereferenceGetter() : get(nullptr) {

    }

    /**
     * Constructs a LuaDerefenceGetter with an existing function.
     *
     * @param getter Function implementing the get & dereferencing from the Lua stack.
     */
    LuaDereferenceGetter(Basetype&(*getter)(LuaStateView&, int)) : get(getter) {

    }

    /**
     * Gets the object in the Lua stack at the specified index, and dereference it into Basetype&.
     *
     * @param state Lua state containing the object.
     * @param stackIndex Index of the object in the stack.
     * @return A reference to the basetype of the object at the specified index.
     */
    Basetype& operator()(LuaStateView& state, int stackIndex) {
        return get(state, stackIndex);
    }
};

// See LuaBinding.hpp
template<typename T>
class LuaBinding<LuaDereferenceGetter<T>,std::void_t<decltype(LuaBinding<T>::luaClassName)>> : public LuaBasicBinding<LuaDereferenceGetter<T>> {
public:
    static const std::string& luaClassName() {
        static const std::string className(std::string("LuaDereferenceGetter<") + LuaBinding<T>::luaClassName() + ">");
        return className;
    }
};

#endif /* LUADEREFERENCEGETTER_HPP */

