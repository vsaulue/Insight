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

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "lua/types/LuaTable.hpp"
#include "Shape.hpp"
#include "units/Scalar.hpp"
#include "units/SI.hpp"

/** Sphere shape. */
class SphereShape : public Shape {
public:
    /**
     * Creates a new sphere shape.
     * @param mass Mass of this shape.
     * @param radius Radius of this sphere.
     */
    SphereShape(Scalar<SI::Mass> mass, Scalar<SI::Length> radius);

    /**
     * Creates a new sphere shape.
     * @param density Density of this shape.
     * @param radius Radius of this sphere.
     */
    SphereShape(Scalar<SI::Density> density, Scalar<SI::Length> radius);

    virtual ~SphereShape();

    /**
     * Gets the radius of this sphere.
     * @return The radius of this sphere.
     */
    Scalar<SI::Length> getRadius() const;

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    /**
     * Constructs a SphereShape object from a Lua table.
     * @param table Lua table containing the parameters of this shape.
     * @return The new SphereShape object.
     */
    static std::unique_ptr<SphereShape> luaGetFromTable(LuaTable& table);
private:
    /** Bullet shape. */
    btSphereShape shape;
};

#endif /* SPHERESHAPE_HPP */
