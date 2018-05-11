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

#ifndef CUBOIDSHAPE_HPP
#define CUBOIDSHAPE_HPP

#include "btBulletDynamicsCommon.h"

#include "Shape.hpp"

/** Cuboid shape. */
class CuboidShape : public Shape {
public:
    /**
     * Creates a new cuboid shape.
     * @param[in] mass Mass of this shape.
     * @param[in] halfExtents Half extents (scale coefficients on each axis) of this new shape.
     */
    CuboidShape(btScalar mass, const btVector3& halfExtents);

    /**
     * Creates a new cuboid shape.
     * @param[in] density Density of this shape.
     * @param[in] halfExtents Half extents (scale coefficients on each axis) of this new shape.
     */
    CuboidShape(Density density, const btVector3& halfExtents);

    virtual ~CuboidShape();

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;
private:
    /** Bullt shape. */
    btBoxShape shape;
};

#endif /* CUBOIDSHAPE_HPP */
