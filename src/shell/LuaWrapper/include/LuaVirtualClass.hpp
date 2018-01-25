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

#ifndef LUAVIRTUALCLASS_HPP
#define LUAVIRTUALCLASS_HPP

class LuaStateView;

#include <string>

/**
 * Interface for C++ types that should support runtime polymorphism with Lua bindings.
 */
class LuaVirtualClass {
public:
    /**
     * Get the class name in Lua of this type.
     *
     * The returned value should be unique for each C++ type wrapped in Lua.
     *
     * @return The name of this class in Lua.
     */
    virtual const std::string& luaClassName() const = 0;

    /**
     * Gets a field or method from this object.
     *
     * @param state Lua state requesting a member to this object.
     * @return The number of returned values in the Lua stack.
     */
    virtual int luaIndex(const std::string& memberName, LuaStateView& state) = 0;

    virtual ~LuaVirtualClass();
};

#include "LuaStateView.hpp"

#endif /* LUAVIRTUALCLASS_HPP */

