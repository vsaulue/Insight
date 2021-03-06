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
    result << "{x=" << object.x() << ", y=" << object.y() << ", z=" << object.z() << "}";
    return result.str();
}

btQuaternion LuaBinding<btQuaternion>::getFromTable(LuaTable& table) {
    using Str = LuaNativeString;
    if (table.has<Str>("x")) {
        btScalar x = table.get<Str,btScalar>("x");
        btScalar y = table.get<Str,btScalar>("y");
        btScalar z = table.get<Str,btScalar>("z");
        btScalar w = table.get<Str,btScalar>("w");
        return btQuaternion(x,y,z,w);
    } else if (table.has<float>(1)) {
        btScalar x = table.get<float,btScalar>(1);
        btScalar y = table.get<float,btScalar>(2);
        btScalar z = table.get<float,btScalar>(3);
        btScalar w = table.get<float,btScalar>(4);
        return btQuaternion(x,y,z,w);
    } else if (table.has<Str>("axis")) {
        btVector3 axis = table.get<Str,btVector3>("axis");
        btScalar angle = table.get<Str,btScalar>("angle");
        return btQuaternion(axis, angle);
    } else {
        throw LuaException("Cannot build btQuaternion from the given Lua table. Requires {x,y,z,w}, {1,2,3,4}, or {axis,angle} fields.");
    }
}

int LuaBinding<btQuaternion>::luaIndexImpl(btQuaternion& object, const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="x") {
        state.push<btScalar>(object.x());
    } else if (memberName=="y") {
        state.push<btScalar>(object.y());
    } else if (memberName=="z") {
        state.push<btScalar>(object.z());
    } else if (memberName=="w") {
        state.push<btScalar>(object.w());
    } else {
        result = 0;
    }
    return result;
}

std::string LuaBinding<btQuaternion>::luaToStringImpl(btQuaternion& object) {
    std::ostringstream result;
    result << "{x=" << object.x() << ", y=" << object.y() << ", z=" << object.z() << ", w=" << object.w() << "}";
    return result.str();
}

template<>
class LuaBinding<btTransform> : public LuaDefaultBinding<btTransform> {
public:
    static btTransform getFromTable(LuaTable& table);

    static int luaIndexImpl(btTransform& object, const std::string& memberName, LuaStateView& state);

    static std::string luaToStringImpl(btTransform& object);
};

btTransform LuaBinding<btTransform>::getFromTable(LuaTable& table) {
    using Str = LuaNativeString;
    btVector3 position = table.get<Str,btVector3>("position");
    btQuaternion rotation = table.get<Str,btQuaternion>("rotation");
    return btTransform(rotation, position);
}

int LuaBinding<btTransform>::luaIndexImpl(btTransform& object, const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="rotation") {
        state.push<btQuaternion>(object.getRotation());
    } else if (memberName=="position") {
        state.push<btVector3>(object.getOrigin());
    } else {
        result = 0;
    }
    return result;
}

std::string LuaBinding<btTransform>::luaToStringImpl(btTransform& object) {
    std::ostringstream result;
    std::string position = LuaBinding<btVector3>::luaToStringImpl(object.getOrigin());
    btQuaternion quat = object.getRotation();
    std::string rotation = LuaBinding<btQuaternion>::luaToStringImpl(quat);
    result << "{position=" << position << ", rotation=" << rotation << "}";
    return result.str();
}

namespace details {
    btTransform LuaTransformBinding::getFromTable(LuaTable& table) {
        return LuaBinding<btTransform>::getFromTable(table);
    }

    int LuaTransformBinding::luaIndexImpl(btTransform& object, const std::string& memberName, LuaStateView& state) {
        return LuaBinding<btTransform>::luaIndexImpl(object, memberName, state);
    }

    std::string LuaTransformBinding::luaToStringImpl(btTransform& object) {
        return LuaBinding<btTransform>::luaToStringImpl(object);
    }

    btVector3 LuaVector3Binding::getFromTable(LuaTable& table) {
        return LuaBinding<btVector3>::getFromTable(table);
    }

    int LuaVector3Binding::luaIndexImpl(btVector3& object, const std::string& memberName, LuaStateView& state) {
        return LuaBinding<btVector3>::luaIndexImpl(object, memberName, state);
    }

    std::string LuaVector3Binding::luaToStringImpl(btVector3& object) {
        return LuaBinding<btVector3>::luaToStringImpl(object);
    }
}