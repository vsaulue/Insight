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

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "lua/types/LuaTable.hpp"
#include "Shape.hpp"
#include "units/Scalar.hpp"
#include "units/SI.hpp"
#include "units/Vector3.hpp"

/** Cuboid shape. */
class CuboidShape : public Shape {
public:
    /**
     * Creates a new cuboid shape.
     * @param[in] mass Mass of this shape.
     * @param[in] halfExtents Half extents (scale coefficients on each axis) of this new shape.
     */
    CuboidShape(Scalar<SI::Mass> mass, const Vector3<SI::Length>& halfExtents);

    /**
     * Creates a new cuboid shape.
     * @param[in] density Density of this shape.
     * @param[in] halfExtents Half extents (scale coefficients on each axis) of this new shape.
     */
    CuboidShape(Scalar<SI::Density> density, const Vector3<SI::Length>& halfExtents);

    virtual ~CuboidShape();

    /**
     * Gets the half extents of this cuboid.
     * @return The half extents of this cuboid.
     */
    Vector3<SI::Length> getHalfExtents() const;

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    /**
     * Creates anew CuboidShape from a Lua table.
     * @param table Lua table containing the parameters of this shape.
     * @return The new CuboidShape.
     */
    static std::unique_ptr<CuboidShape> luaGetFromTable(LuaTable& table);
private:
    /** Bullt shape. */
    btBoxShape shape;
};

#endif /* CUBOIDSHAPE_HPP */
