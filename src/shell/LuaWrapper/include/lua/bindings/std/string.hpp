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

#ifndef STRING_HPP
#define STRING_HPP

#include <string>

#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/LuaStateView.hpp"

template<>
class LuaBinding<std::string> : public LuaDefaultBinding<std::string> {
public:
    static const std::string& luaClassName() {
        static const std::string className("std::string");
        return className;
    }

    static std::string get(LuaStateView& state, int stackIndex) {
        if (state.isString(stackIndex)) {
            return state.get<LuaNativeString>(stackIndex);
        } else {
            return LuaDefaultBinding::get(state, stackIndex);
        }
    }

    static std::string luaToStringImpl(std::string& object) {
        // TODO: escape double quotes & backslashes.
        return std::string("\"") + object + '"';
    }
};

#endif /* STRING_HPP */
