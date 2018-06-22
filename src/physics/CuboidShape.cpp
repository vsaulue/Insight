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

#include "CuboidShape.hpp"
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "units/BulletUnits.hpp"

CuboidShape::CuboidShape(Scalar<SI::Mass> mass, const Vector3<SI::Length>& halfExtents) :
    Shape(mass),
    shape(toBulletUnits(halfExtents))
{

}

/**
 * Computes the volume of a cuboid.
 * @param[in] halfExtents Half extents of the cuboid.
 * @return The volume of the cuboid.
 */
static Scalar<SI::Volume> cuboidVolume(const Vector3<SI::Length>& halfExtents) {
    return btScalar(8)*halfExtents.x()*halfExtents.y()*halfExtents.z();
}

CuboidShape::CuboidShape(Scalar<SI::Density> density, const Vector3<SI::Length>& halfExtents) :
    CuboidShape(cuboidVolume(halfExtents) * density, halfExtents)
{

}

CuboidShape::~CuboidShape() = default;

Vector3<SI::Length> CuboidShape::getHalfExtents() const {
    return fromBulletValue<SI::Length>(shape.getHalfExtentsWithMargin());
}

btCollisionShape& CuboidShape::getBulletShape() {
    return shape;
}

const btCollisionShape& CuboidShape::getBulletShape() const {
    return shape;
}

void CuboidShape::draw(ShapeDrawer& drawer, const btTransform& transform) const {
    btVector3 halfExtents = shape.getHalfExtentsWithMargin();
    drawer.drawCuboid(transform, halfExtents);
}

int CuboidShape::luaIndex(const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="halfExtents") {
        state.push<Vector3<SI::Length>>(getHalfExtents());
    } else {
        result = Shape::luaIndex(memberName, state);
    }
    return result;
}

std::unique_ptr<CuboidShape> CuboidShape::luaGetFromTable(LuaTable& table) {
    auto halfExtents = table.get<LuaNativeString,Vector3<SI::Length>>("halfExtents");
    Scalar<SI::Mass> mass;
    if (table.has<LuaNativeString>("mass")) {
        mass = table.get<LuaNativeString,Scalar<SI::Mass>>("mass");
    } else {
        auto density = table.get<LuaNativeString,Scalar<SI::Density>>("density");
        mass = density * cuboidVolume(halfExtents);
    }
    return std::make_unique<CuboidShape>(mass, halfExtents);
}
