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

#ifndef COMPOUNDBODY_HPP
#define COMPOUNDBODY_HPP

#include <memory>
#include <unordered_set>

#include "btBulletDynamicsCommon.h"

#include "Body.hpp"

/** Concave object, composed of multiple convex shapes. */
class CompoundBody : public Body {
public:
    /**
     * Creates a new empty CompoundBody().
     *
     * Convex shapes can be added with the add* methods.
     */
    CompoundBody();

    virtual ~CompoundBody();

    /** Adds a new sphere in this collision shape.
     *
     * @param[in] radius Radius of the sphere.
     * @param[in] center Center of the sphere.
     */
    void addSphere(btScalar radius, const btVector3& center);

    /**
     * Adds a new cylinder in this collision shape.
     *
     * With a default transform, the cylinder has a central axis aligned with the Y axis.
     * It is centered on {0,0,0}.
     *
     * With default halfExtents {1,1,1}, it has a radius of 1 and a length of 2.
     *
     * @param[in] transform Translation & rotation of the new cylinder.
     * @param[in] halfExtents Scale coefficient on each axis of the cylinder.
     */
    void addCylinder(const btTransform& transform, const btVector3& halfExtents);

    void drawShape(ShapeDrawer& drawer) const override;

    /** Convex shape belonging to a CompoundBody. */
    class Child;
private:
    /** Internal constructor, using an existing Bullet shape. */
    CompoundBody(std::unique_ptr<btCompoundShape> shape);

    /** Bullet representation of the concave shape.*/
    std::unique_ptr<btCompoundShape> shape;

    /** List of convex objects of this Body.*/
    std::unordered_set<std::unique_ptr<Child>> children;
};

#endif /* COMPOUNDBODY_HPP */
