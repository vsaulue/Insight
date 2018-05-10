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

/**
 * @file src/physics/bullet.cpp
 * Contains implementations of Lua bindings of Bullet classes.
 */

#include <sstream>

#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaNativeString.hpp"

btVector3 LuaBinding<btVector3>::getFromTable(LuaTable& table) {
    using Str = LuaNativeString;
    if (table.has<Str>("x")) {
        return {
            table.get<Str, btScalar>("x"),
            table.get<Str, btScalar>("y"),
            table.get<Str, btScalar>("z")
        };
    } else if (table.has<float>(1)) {
        return {
            table.get<float, btScalar>(1),
            table.get<float, btScalar>(2),
            table.get<float, btScalar>(3)
        };
    } else {
        throw LuaException("Cannot build btVector3 from the given Lua table. Requires \"x\",\"y\",\"z\" or 1,2,3 fields.");
    }
}

int LuaBinding<btVector3>::luaIndexImpl(btVector3& object, const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="x") {
        state.push<btScalar>(object.x());
    } else if (memberName=="y") {
        state.push<btScalar>(object.y());
    } else if (memberName=="z") {
        state.push<btScalar>(object.z());
    } else {
        result = 0;
    }
    return result;
}

std::string LuaBinding<btVector3>::luaToStringImpl(btVector3& object) {
    std::ostringstream result;
    result << luaClassName() << ": {x=" << object.x() << ", y=" << object.y() << ", z=" << object.z() << "}";
    return result.str();
}
