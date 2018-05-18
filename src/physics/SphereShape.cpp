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

#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "lua/types/LuaTable.hpp"
#include "SphereShape.hpp"

SphereShape::SphereShape(btScalar mass, btScalar radius) :
    Shape(mass),
    shape(radius)
{

}

/**
 * Computes the volume of a sphere.
 * @param[in] radius Radius of the sphere.
 * @return The volume of the sphere.
 */
static btScalar sphereVolume(btScalar radius) {
    return SIMD_PI*btScalar(4)/btScalar(3)*radius*radius*radius;
}

SphereShape::SphereShape(Density density, btScalar radius) :
    SphereShape(sphereVolume(radius)*density.value, radius)
{

}

SphereShape::~SphereShape() = default;

btScalar SphereShape::getRadius() const {
    return shape.getRadius();
}


btCollisionShape& SphereShape::getBulletShape() {
    return shape;
}

const btCollisionShape& SphereShape::getBulletShape() const {
    return shape;
}

void SphereShape::draw(ShapeDrawer& drawer, const btTransform& transform) const {
    drawer.drawSphere(transform.getOrigin(), shape.getRadius());
}

int SphereShape::luaIndex(const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="radius") {
        state.push<btScalar>(getRadius());
    } else {
        result = Shape::luaIndex(memberName, state);
    }
    return result;
}

std::unique_ptr<SphereShape> SphereShape::luaGetFromTable(LuaTable& table) {
    btScalar radius = table.get<LuaNativeString,btScalar>("radius");
    btScalar mass;
    if (table.has<LuaNativeString>("mass")) {
        mass = table.get<LuaNativeString,btScalar>("mass");
    } else {
        btScalar density = table.get<LuaNativeString,btScalar>("density");
        mass = density * sphereVolume(radius);
    }
    return std::make_unique<SphereShape>(mass, radius);
}
