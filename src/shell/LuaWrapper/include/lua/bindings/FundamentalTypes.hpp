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

#ifndef LUABINDINGFUNDAMENTALTYPES_HPP
#define LUABINDINGFUNDAMENTALTYPES_HPP

#include <type_traits>

#include "lua/bindings/helpers/LuaDefaultClassName.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/LuaStateView.hpp"

/** See LuaBinding in LuaBinding.hpp. */
template<>
class LuaBinding<bool> : public LuaDefaultClassName<bool> {
public:
    static void push(LuaStateView& state, bool value) {
        state.pushBool(value);
    }

    static bool get(LuaStateView& state, int stackIndex) {
        return state.getBool(stackIndex);
    }
};

/** See LuaBinding in LuaBinding.hpp. */
template<typename T>
class LuaBinding<T, typename std::enable_if<std::is_floating_point<T>::value && sizeof(T) <= sizeof(double)>::type> : public LuaDefaultClassName<T> {
public:
    static void push(LuaStateView& state, T value) {
        state.pushDouble(value);
    }

    static T get(LuaStateView& state, int stackIndex) {
        return state.getDouble(stackIndex);
    }
};

#endif /* LUABINDINGFUNDAMENTALTYPES_HPP */

