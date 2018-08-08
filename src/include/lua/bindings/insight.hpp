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

#ifndef LUA_BINDINGS_INSIGHT_HPP
#define LUA_BINDINGS_INSIGHT_HPP

#include <string>

#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/LuaBinding.hpp"
#include "Robot.hpp"

template<>
class LuaBinding<Robot> : public LuaDefaultBinding<Robot> {
public:
    static int luaIndexImpl(Robot& object, const std::string& memberName, LuaStateView& state);

    static std::string luaToStringImpl(Robot& object);
};

#endif /* LUA_BINDINGS_INSIGHT_HPP */
