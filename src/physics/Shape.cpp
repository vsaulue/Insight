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

#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"

#include "Shape.hpp"
#include "CompoundShape.hpp"
#include "ConvexHullShape.hpp"
#include "CuboidShape.hpp"
#include "CylinderShape.hpp"
#include "SphereShape.hpp"
#include "StaticPlaneShape.hpp"
#include "units/BulletUnits.hpp"


Shape::Shape(Scalar<SI::Mass> mass) : mass(mass) {

}

btVector3 Shape::getEngineInertia() const {
    btVector3 inertia;
    btScalar bulletMass = toBulletUnits(mass);
    if (bulletMass > 0) {
        getBulletShape().calculateLocalInertia(bulletMass, inertia);
    } else {
        inertia={0,0,0};
    }
    return inertia;
}

Vector3<SI::AngularMass> Shape::getInertia() const {
    return fromBulletValue<SI::AngularMass>(getEngineInertia());
}

int Shape::luaIndex(const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="mass") {
        state.push<Scalar<SI::Mass>>(mass);
    } else if (memberName=="inertia") {
        state.push<Vector3<SI::AngularMass>>(getInertia());
    } else {
        result = 0;
    }
    return result;
}

std::unique_ptr<Shape> Shape::luaGetFromTable(LuaTable& table) {
    std::string type = table.get<LuaNativeString, LuaNativeString>("type");
    LuaTable params = table.get<LuaNativeString, LuaTable>("params");
    if (type=="Compound") {
        return CompoundShape::luaGetFromTable(params);
    } else if (type=="ConvexHull") {
        return ConvexHullShape::luaGetFromTable(params);
    } else if (type=="Cuboid") {
        return CuboidShape::luaGetFromTable(params);
    } else if (type=="Cylinder") {
        return CylinderShape::luaGetFromTable(params);
    } else if (type=="Sphere") {
        return SphereShape::luaGetFromTable(params);
    } else if (type=="StaticPlane") {
        return StaticPlaneShape::luaGetFromTable(params);
    } else {
        std::string msg = std::string("Invalid 'type' field in Shape table constructor: ") + type;
        throw LuaException(msg.c_str());
    }
}
