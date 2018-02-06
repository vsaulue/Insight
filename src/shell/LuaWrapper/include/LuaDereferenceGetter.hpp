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

template<typename Basetype>
class LuaDereferenceGetter {
private:
    Basetype&(*get)(LuaStateView&, int);
public:
    LuaDereferenceGetter() : get(nullptr) {

    }

    LuaDereferenceGetter(Basetype&(*getter)(LuaStateView&, int)) : get(getter) {

    }

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

