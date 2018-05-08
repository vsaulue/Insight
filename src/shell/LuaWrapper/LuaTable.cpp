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

#include <sstream>

#include "lua/LuaException.hpp"
#include "lua/types/LuaTable.hpp"

LuaTable::LuaTable(LuaStateView& state) :
    state(state)
{
    state.newTable();
    stackIndex = state.getTop();
}

LuaTable::LuaTable(LuaStateView& state, int stackIndex) :
    stackIndex(state.absIndex(stackIndex)),
    state(state)
{
    if (!state.isTable(stackIndex)) {
        std::stringstream msg;
        msg << "Expected Lua table at index " << stackIndex << ". Found type " << state.getTypename(stackIndex) << ".";
        throw LuaException(msg.str().c_str());
    }
}

LuaTable::LuaTable(LuaTable&& table) :
    stackIndex(table.stackIndex),
    state(table.state)
{
    table.stackIndex = 0;
}

LuaTable::~LuaTable() {
    if (stackIndex != 0 && stackIndex == state.getTop()) {
        state.pop(1);
    }
}
