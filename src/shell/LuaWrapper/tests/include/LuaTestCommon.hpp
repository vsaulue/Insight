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

#ifndef LUATESTCOMMON_HPP
#define LUATESTCOMMON_HPP

#include "lua/LuaStateView.hpp"

/**
 * Defines the global function "readBool" in Lua, reading a Lua bool into a C++ variable.
 *
 * @param state Lua state in which the read function will be defined.
 * @param res Reference to the output variable.
 */
void defineReadBool(LuaStateView& state, bool& res);

/**
 * Defines the global function "readFloat" in Lua, reading a Lua number into a C++ variable.
 *
 * @param state Lua state in which the read function will be defined.
 * @param res Reference to the output variable.
 */
void defineReadFloat(LuaStateView& state, float& res);

#endif /* LUATESTCOMMON_HPP */
