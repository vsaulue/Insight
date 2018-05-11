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

CuboidShape::CuboidShape(btScalar mass, const btVector3& halfExtents) :
    Shape(mass),
    shape(halfExtents)
{

}

/**
 * Computes the volume of a cuboid.
 * @param[in] halfExtents Half extents of the cuboid.
 * @return The volume of the cuboid.
 */
static btScalar cuboidVolume(const btVector3& halfExtents) {
    return btScalar(8)*halfExtents.x()*halfExtents.y()*halfExtents.z();
}

CuboidShape::CuboidShape(Density density, const btVector3& halfExtents) :
    CuboidShape(cuboidVolume(halfExtents)*density.value, halfExtents)
{

}

CuboidShape::~CuboidShape() = default;

btCollisionShape& CuboidShape::getBulletShape() {
    return shape;
}

const btCollisionShape& CuboidShape::getBulletShape() const {
    return shape;
}

void CuboidShape::draw(ShapeDrawer& drawer, const btTransform& transform) const {
    drawer.drawCuboid(transform, shape.getHalfExtentsWithoutMargin());
}
