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

#ifndef SHAPEDRAWER_HPP
#define SHAPEDRAWER_HPP

#include "btBulletDynamicsCommon.h"

/** 
 * Interface for reading the geometry of a Body outside the Physics engine.
 */
class ShapeDrawer {
public:
    /**
     * Draw a sphere.
     *
     * @param[in] center Center of the sphere (relative to the center of mass of the Body).
     * @param[in] radius Radius of the sphere.
     */
    virtual void drawSphere(const btVector3& center, btScalar radius) = 0;

    /**
     * Draw an infinite plane.
     *
     * @param[in] normal Normal vector of the surface (relative to the orientation of the Body).
     * @param[in] offset Offset of the plane from the origin (= center of mass of the Body), on the normal vector.
     */
    virtual void drawPlane(const btVector3& normal, btScalar offset) = 0;

    /**
     * Draw a cylinder.
     *
     * With a default transform, the cylinder has a central axis aligned with the Y axis.
     * It is centered on {0,0,0}.
     *
     * With default halfExtents {1,1,1}, it has a radius of 1 and a length of 2.
     *
     * @param[in] transform Position & orientation of the cylinder.
     * @param[in] halfExtents Half extents (scale coefficients on each axis).
     */
    virtual void drawCylinder(const btTransform& transform, const btVector3& halfExtents) = 0;

    /**
     * Draw a cuboid.
     *
     * @param[in] transform Position & orientation of the cuboid.
     * @param[in] halfExtents Half extents (scale coefficients on each axis).
     */
    virtual void drawCuboid(const btTransform& transform, const btVector3& halfExtents) = 0;

    virtual ~ShapeDrawer() = default;
};

#endif /* SHAPEDRAWER_HPP */

