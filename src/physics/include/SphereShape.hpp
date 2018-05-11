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

#ifndef SPHERESHAPE_HPP
#define SPHERESHAPE_HPP

#include "btBulletDynamicsCommon.h"

#include "Shape.hpp"

/** Sphere shape. */
class SphereShape : public Shape {
public:
    /**
     * Creates a new sphere shape.
     * @param mass Mass of this shape.
     * @param radius Radius of this sphere.
     */
    SphereShape(btScalar mass, btScalar radius);

    /**
     * Creates a new sphere shape.
     * @param density Density of this shape.
     * @param radius Radius of this sphere.
     */
    SphereShape(Density density, btScalar radius);

    virtual ~SphereShape();

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;
private:
    /** Bullet shape. */
    btSphereShape shape;
};

#endif /* SPHERESHAPE_HPP */
