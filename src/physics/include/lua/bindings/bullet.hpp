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

#ifndef LUA_BINDINGS_BULLET_HPP
#define LUA_BINDINGS_BULLET_HPP

#include <string>

#include "btBulletDynamicsCommon.h"

#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/types/LuaTable.hpp"

template<>
class LuaBinding<btVector3> : public LuaDefaultBinding<btVector3> {
public:
    static btVector3 getFromTable(LuaTable& table);

    static int luaIndexImpl(btVector3& object, const std::string& memberName, LuaStateView& state);

    static std::string luaToStringImpl(btVector3& object);
};

template<>
class LuaBinding<btQuaternion> : public LuaDefaultBinding<btQuaternion> {
public:
    static btQuaternion getFromTable(LuaTable& table);

    static int luaIndexImpl(btQuaternion& object, const std::string& memberName, LuaStateView& state);

    static std::string luaToStringImpl(btQuaternion& object);
};

#endif /* LUA_BINDINGS_BULLET_HPP */
