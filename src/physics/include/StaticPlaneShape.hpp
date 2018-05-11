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

#ifndef STATICPLANESHAPE_HPP
#define STATICPLANESHAPE_HPP

#include "btBulletDynamicsCommon.h"

#include "Shape.hpp"

/** Static plane shape. */
class StaticPlaneShape : public Shape {
public:
    /** Creates a new static plane shape.
     * @param normal Normal vector of the plane.
     * @param offset Offset of the plane (the point origin+Offset*Normal belongs to the plane).
     */
    StaticPlaneShape(const btVector3& normal, btScalar offset);

    virtual ~StaticPlaneShape();

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;
private:
    /** Bullet shape. */
    btStaticPlaneShape shape;
};

#endif /* STATICPLANESHAPE_HPP */
