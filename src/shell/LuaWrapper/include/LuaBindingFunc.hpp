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

#ifndef LUABINDINGFUNC_HPP
#define LUABINDINGFUNC_HPP

#include "LuaBinding.hpp"
#include "LuaDefaultBinding.hpp"

/** See LuaBinding in LuaBinding.hpp. */
template<>
class LuaBinding<int(*)(LuaStateView&)> : public LuaDefaultBinding<int(*)(LuaStateView&)> {
public:
    /**
     * Get the name of the metatable of this type.
     *
     * @return The name of this C++ type in Lua.
     */
    static const std::string& luaClassName() {
        static const std::string className("int(*)(LuaStateView&)");
        return className;
    }
};

#endif /* LUABINDINGFUNC_HPP */

