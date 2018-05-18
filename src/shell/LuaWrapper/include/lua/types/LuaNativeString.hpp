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

#ifndef LUANATIVESTRING_HPP
#define LUANATIVESTRING_HPP

#include "lua/LuaStateView.hpp"

/** Wrapper of a native Lua string. */
struct LuaNativeString {
    /** Pointer to the string stored inside Lua. */
    const char* value;

    /**
     * Constructs a new object that can be converted to/from a Lua native string.
     * @param value Value of the string.
     */
    LuaNativeString(const char* value) : value(value) {

    }

    /**
     * Returns a pointer to the stored value.
     * @return Pointer to the wrapped string.
     */
    operator const char*() const {
        return value;
    }

    /**
     * Constructs a copy of the string value.
     * @return A copy of the wrapped string.
     */
    operator std::string() const {
        return value;
    }
};

/** See LuaBinding in LuaBinding.hpp*/
template<>
class LuaBinding<LuaNativeString> {
public:
    static void push(LuaStateView& state, LuaNativeString value) {
        state.pushString(value);
    }

    static LuaNativeString get(LuaStateView& state, int stackIndex) {
        return state.getString(stackIndex);
    }
};

#endif /* LUANATIVESTRING_HPP */
