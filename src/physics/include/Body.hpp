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
#include "lua/types/LuaTable.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "Shape.hpp"
#include "ShapeDrawer.hpp"
#include "units/SI.hpp"
#include "units/Vector3.hpp"
#include "WorldUpdater.hpp"

/** Object of the world (physics engine). */
class Body : public LuaVirtualClass {
public:
    /**
     * Constructs a new Body.
     *
     * @param shape Shape of the body (used for collisions).
     */
    Body(std::shared_ptr<Shape> shape);

    /**
     * Gets the position of this object.
     * @return The position of this object.
     */
    Vector3<SI::Length> getPosition() const;

    /**
     * Sets the position of this object.
     * @param newPos The new position of this object.
     */
    void setPosition(const Vector3<SI::Length>& newPos);

    /**
     * Sets the orientation of this object.
     * @param[in] newRotation The new rotation of this object.
     */
    void setRotation(const btQuaternion& newRotation);

    /**
     * Sets the position & orientation of this object.
     * @param[in] transform The new position & orientation of this object.
     */
    void setEngineTransform(const btTransform& transform);

    /**
     * Gets the position & orientation of this object in engine units.
     * @return The current position & orientation of this object.
     */
    const btTransform& getEngineTransform() const;

    /**
     * Gets the linear velocity of this object in engine units.
     * @return The linear velocity of this object.
     */
    Vector3<SI::Speed> getLinearVelocity() const;

    /**
     * Sets the linear velocity of this object.
     * @param velocity The new linear velocity of this object.
     */
    void setLinearVelocity(const Vector3<SI::Speed>& velocity);

    /**
     * Gets a reference to the Bullet representation of this Body.
     *
     * @return A reference to the internal Bullet Body.
     */
    btRigidBody& getBulletBody();

    /**
     * Sets the world udpater of this object.
     *
     * @param newValue
     */
    void setWorldUpdater(WorldUpdater* newValue);

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    static std::unique_ptr<Body> luaGetFromTable(LuaTable& table);

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
    /** World callbacks to produce specific events when this object is in a world (can be null). */
    WorldUpdater* worldUpdater;
};

#endif /* BODY_HPP */
