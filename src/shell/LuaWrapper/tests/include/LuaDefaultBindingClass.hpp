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

#ifndef LUADEFAULTBINDINGCLASS_HPP
#define LUADEFAULTBINDINGCLASS_HPP

#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/types/LuaTable.hpp"

/**
 * A simple C++ class binded to Lua using LuaDefaultBinding.
 */
class LuaDefaultBindingClass {
public:
    /** Number of destructor calls. */
    static int deleteCounter;
    /** Number of constructor calls. */
    static int createCounter;
    /**
     * Constructs a new instance.
     * @param value Initial value of the value field.
     */
    LuaDefaultBindingClass(float value);

    /** Copy constructor. */
    LuaDefaultBindingClass(const LuaDefaultBindingClass& obj);

    /** Move constructor. */
    LuaDefaultBindingClass(LuaDefaultBindingClass&& obj);

    ~LuaDefaultBindingClass();

    /** A float value. */
    float value;

    /**
     * Implementation of Lua __call metamethod for this type.
     *
     * In Lua, it reads a single float, and return the opposite.
     *
     * @param state Lua State calling this method.
     * @return Number of return values on the Lua stack.
     */
    int operator()(LuaStateView& state);
};

template<>
class LuaBinding<LuaDefaultBindingClass> : public LuaDefaultBinding<LuaDefaultBindingClass> {
public:
    static int luaIndexImpl(LuaDefaultBindingClass& object, const std::string& memberName, LuaStateView& state);

    static LuaDefaultBindingClass getFromTable(LuaTable& table);
};

#endif /* LUADEFAULTBINDINGCLASS_HPP */
