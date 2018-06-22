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
#include "StaticPlaneShape.hpp"
#include "units/BulletUnits.hpp"

StaticPlaneShape::StaticPlaneShape(const Vector3<SI::NoUnit>& normal, Scalar<SI::Length> offset) :
    Shape(Scalar<SI::Mass>(0)),
    shape(toBulletUnits(normal), toBulletUnits(offset))
{

}

StaticPlaneShape::~StaticPlaneShape() = default;

Vector3<SI::NoUnit> StaticPlaneShape::getNormal() const {
    return fromBulletValue<SI::NoUnit>(shape.getPlaneNormal());
}

Scalar<SI::Length> StaticPlaneShape::getOffset() const {
    return fromBulletValue<SI::Length>(shape.getPlaneConstant());
}

btCollisionShape& StaticPlaneShape::getBulletShape() {
    return shape;
}

const btCollisionShape& StaticPlaneShape::getBulletShape() const {
    return shape;
}

void StaticPlaneShape::draw(ShapeDrawer& drawer, const btTransform& transform) const {
    btVector3 absNormal = transform.getBasis() * shape.getPlaneNormal();
    btScalar absOffset = shape.getPlaneConstant() + absNormal.dot(transform.getOrigin());
    drawer.drawPlane(absNormal, absOffset);
}

int StaticPlaneShape::luaIndex(const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="normal") {
        state.push<Vector3<SI::NoUnit>>(getNormal());
    } else if (memberName=="offset") {
        state.push<Scalar<SI::Length>>(getOffset());
    } else {
        result = Shape::luaIndex(memberName, state);
    }
    return result;
}

std::unique_ptr<StaticPlaneShape> StaticPlaneShape::luaGetFromTable(LuaTable& table) {
    auto normal = table.get<LuaNativeString,Vector3<SI::NoUnit>>("normal");
    auto offset = table.get<LuaNativeString,Scalar<SI::Length>>("offset");
    return std::make_unique<StaticPlaneShape>(normal, offset);
}
