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

#ifndef BODY_HPP
#define BODY_HPP

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "BodyMoveListener.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "Shape.hpp"
#include "ShapeDrawer.hpp"

/** Object of the world (physics engine). */
class Body : public LuaVirtualClass {
public:
    /**
     * Constructs a new Body.
     *
     * @param shape Shape of the body (used for collisions).
     */
    Body(std::shared_ptr<Shape> shape);

    void setPosition(const btVector3& newPos);

    /**
     * Sets the orientation of this object.
     * @param[in] newRotation The new rotation of this object.
     */
    void setRotation(const btQuaternion& newRotation);

    /**
     * Sets the position & orientation of this object.
     * @param[in] transform The new position & orientation of this object.
     */
    void setTransform(const btTransform& transform);

    /**
     * Gets the position & orientation of this object.
     * @return The current position & orientation of this object.
     */
    const btTransform& getTransform() const;

    /**
     * Gets a reference to the Bullet representation of this Body.
     *
     * @return A reference to the internal Bullet Body.
     */
    btRigidBody& getBulletBody();

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    /**
     * Draws the collision shape of this Body into the specified Drawer object.
     *
     * A Body will be represented by one or several "basic" collision shapes (sphere,
     * plane)
     *
     * @param drawer Object in which the shape should be drawn.
     */
    void drawShape(ShapeDrawer& drawer) const {
        shape->draw(drawer);
    }

    const Shape& getShape() const {
        return *shape;
    }

    /**
     * Adds a new listener for "move" events.
     *
     * @param listener The new listener.
     */
    void addMoveListener(BodyMoveListener& listener) const;

    /**
     * Removes listener for "move" events.
     *
     * @param listener The listener to remove.
     */
    void removeMoveListener(BodyMoveListener& listener) const;

    virtual ~Body();
private:
    class MotionState;

    /** Shape of this body. */
    std::shared_ptr<Shape> shape;
    /** Object used by Bullet to communicate position & direction changes. */
    mutable std::unique_ptr<MotionState> motionState;
    /** Bullet body. */
    btRigidBody body;
};

#endif /* BODY_HPP */
