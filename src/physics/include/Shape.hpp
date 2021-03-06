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

#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "lua/types/LuaTable.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "ShapeDrawer.hpp"
#include "units/Scalar.hpp"
#include "units/SI.hpp"
#include "units/Vector3.hpp"

/**
 * Abstract class holding a shape in the physics engine.
 *
 * This class wraps a Bullet btCollisionShape (geometry of the object), and also
 * holds mass & inertia data of this shape.
 */
class Shape : public LuaVirtualClass {
public:
    /**
     * Creates a new shape.
     * @param[in] mass Mass of this object.
     */
    Shape(Scalar<SI::Mass> mass);

    virtual ~Shape() = default;

    /**
     * Gets the Bullet collision shape of this object.
     * @return The collision shape of this object.
     */
    virtual btCollisionShape& getBulletShape() = 0;

    /**
     * Gets the Bullet collision shape of this object.
     * @return The collision shape of this object.
     */
    virtual const btCollisionShape& getBulletShape() const = 0;

    /**
     * Draws this shape into the given drawer.
     * @param drawer Object in which this shape should be drawn.
     * @param transform Transform of this shape (and its children, if any).
     */
    virtual void draw(ShapeDrawer& drawer, const btTransform& transform = btTransform::getIdentity()) const = 0;

    /**
     * Gets the mass of this shape.
     * @return The mass of this shape.
     */
    Scalar<SI::Mass> getMass() const {
        return mass;
    }

    /**
     * Gets the moments of inertia of this shape, in engine units.
     * @return The inertia vector of this shape.
     */
    btVector3 getEngineInertia() const;

    /**
     * Gets the moments of inertia of this shape.
     * @return The inertia vector of this shape.
     */
    Vector3<SI::AngularMass> getInertia() const;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    /**
     * Constructs a Shape type from a Lua table.
     * @return A Shape (or derived type) constructed from the content of the table.
     */
    static std::unique_ptr<Shape> luaGetFromTable(LuaTable& table);
protected:
    /** Mass of this shape. */
    Scalar<SI::Mass> mass;
    /**
     * Non-initializing constructor.
     *
     * Derived classes must initialize the following fields:
     * - mass
     */
    Shape() = default;
};

#endif /* SHAPE_HPP */
