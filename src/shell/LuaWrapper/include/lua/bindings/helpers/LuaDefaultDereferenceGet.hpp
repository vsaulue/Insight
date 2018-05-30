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

#ifndef LUADEFAULTDEREFERENCEGET_HPP
#define LUADEFAULTDEREFERENCEGET_HPP

#include <string>

#include "lua/helpers/LuaAutoPop.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/LuaException.hpp"
#include "lua/LuaStateView.hpp"
#include "lua/types/LuaDereferenceGetter.hpp"

/**
 * Provides a default implementation of LuaBinding dereferenceGet function.
 * @param state Lua stack containing the value to dereference.
 * @param stackIndex Index of the value to derefence.
 * @return A reference to the base type of the value at the specified index.
 */
template<typename Basetype>
Basetype& luaDefaultDereferenceGet(LuaStateView& state, int stackIndex) {
    if(!state.pushMetafield(stackIndex, "dereferenceGetter")) {
        state.throwArgError(stackIndex, std::string("no metamethod to dereference to base type: ") + LuaBinding<Basetype>::luaClassName());
    }
    LuaAutoPop autoPop(state, 1);
    LuaDereferenceGetter<Basetype> getter;
    try {
        getter = state.get<LuaDereferenceGetter<Basetype>>(-1);
    } catch (const LuaException& e) {
        state.throwArgError(stackIndex, e.what());
    }
    return getter(state, stackIndex);
}

#endif /* LUADEFAULTDEREFERENCEGET_HPP */

