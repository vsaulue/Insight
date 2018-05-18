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
#include "lua/types/LuaNativeString.hpp"
#include "CylinderShape.hpp"

CylinderShape::CylinderShape(btScalar mass, const btVector3& halfExtents) :
    Shape(mass),
    shape(halfExtents)
{

}

/**
 * Computes the volume of a cylinder.
 * @param[in] halfExtents Half extents of the cylinder.
 * @return The volume of the cylinder.
 */
static btScalar cylinderVolume(const btVector3& halfExtents) {
    return SIMD_PI*halfExtents.x()*halfExtents.z() * 2*halfExtents.y();
}

CylinderShape::CylinderShape(Density density, const btVector3& halfExtents) :
    CylinderShape(cylinderVolume(halfExtents)*density.value, halfExtents)
{

}

CylinderShape::~CylinderShape() = default;

const btVector3& CylinderShape::getHalfExtents() const {
    return shape.getHalfExtentsWithoutMargin();
}

btCollisionShape& CylinderShape::getBulletShape() {
    return shape;
}

const btCollisionShape& CylinderShape::getBulletShape() const {
    return shape;
}

void CylinderShape::draw(ShapeDrawer& drawer, const btTransform& transform) const {
    drawer.drawCylinder(transform, shape.getHalfExtentsWithoutMargin());
}

int CylinderShape::luaIndex(const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="halfExtents") {
        state.push<btVector3>(getHalfExtents());
    } else {
        result = Shape::luaIndex(memberName, state);
    }
    return result;
}

std::unique_ptr<CylinderShape> CylinderShape::luaGetFromTable(LuaTable& table) {
    btVector3 halfExtents = table.get<LuaNativeString,btVector3>("halfExtents");
    btScalar mass;
    if (table.has<LuaNativeString>("mass")) {
        mass = table.get<LuaNativeString,btScalar>("mass");
    } else {
        btScalar density = table.get<LuaNativeString,btScalar>("density");
        mass = density * cylinderVolume(halfExtents);
    }
    return std::make_unique<CylinderShape>(mass, halfExtents);
}
