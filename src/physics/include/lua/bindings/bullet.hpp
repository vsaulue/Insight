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

#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/bindings/units/Floats.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/types/LuaTable.hpp"
#include "units/framework/UnitSymbols.hpp"
#include "units/Transform.hpp"
#include "units/Vector3.hpp"

template<>
class LuaBinding<btQuaternion> : public LuaDefaultBinding<btQuaternion> {
public:
    static btQuaternion getFromTable(LuaTable& table);

    static int luaIndexImpl(btQuaternion& object, const std::string& memberName, LuaStateView& state);

    static std::string luaToStringImpl(btQuaternion& object);
};

namespace details {
    struct LuaTransformBinding {
        static btTransform getFromTable(LuaTable& table);

        static int luaIndexImpl(btTransform& object, const std::string& memberName, LuaStateView& state);

        static std::string luaToStringImpl(btTransform& object);
    };
}

template<typename Unit>
class LuaBinding<Transform<Unit>> : public LuaDefaultBinding<Transform<Unit>> {
public:
    static const std::string& luaClassName() {
        static const std::string className(std::string("Transform<") + Units::unitSymbol<Unit>() + ">");
        return className;
    }

    static Transform<Unit> getFromTable(LuaTable& table) {
        return Transform<Unit>(details::LuaTransformBinding::getFromTable(table));
    }

    static int luaIndexImpl(Transform<Unit>& object, const std::string& memberName, LuaStateView& state) {
        return details::LuaTransformBinding::luaIndexImpl(object.value, memberName, state);
    }

    static std::string luaToStringImpl(Transform<Unit>& object) {
        return details::LuaTransformBinding::luaToStringImpl(object.value);
    }
};

namespace details {
    struct LuaVector3Binding {
        static btVector3 getFromTable(LuaTable& table);

        static int luaIndexImpl(btVector3& object, const std::string& memberName, LuaStateView& state);

        static std::string luaToStringImpl(btVector3& object);
    };
}

template<typename Unit>
class LuaBinding<Vector3<Unit>> : public LuaDefaultBinding<Vector3<Unit>> {
public:
    static const std::string& luaClassName() {
        static const std::string className(std::string("Vector3<") + Units::unitSymbol<Unit>() + ">");
        return className;
    }

    static Vector3<Unit> getFromTable(LuaTable& table) {
        return Vector3<Unit>(details::LuaVector3Binding::getFromTable(table));
    }

    static int luaIndexImpl(Vector3<Unit>& object, const std::string& memberName, LuaStateView& state) {
        return details::LuaVector3Binding::luaIndexImpl(object.value, memberName, state);
    }

    static std::string luaToStringImpl(Vector3<Unit>& object) {
        return details::LuaVector3Binding::luaToStringImpl(object.value);
    }
};

#endif /* LUA_BINDINGS_BULLET_HPP */
