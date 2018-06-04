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

#ifndef CONVEXHULLSHAPE_HPP
#define CONVEXHULLSHAPE_HPP

#include <memory>
#include <vector>

#include "btBulletDynamicsCommon.h"

#include "lua/types/LuaTable.hpp"
#include "Shape.hpp"

/** Generic shape for convex meshes. */
class ConvexHullShape : public Shape {
public:
    /**
     * Creates a new ConvexHullShape.
     * @param mass Mass of the shape.
     * @param vertices Vertices of the convex mesh.
     */
    ConvexHullShape(btScalar mass, const std::vector<btVector3>& vertices);

    virtual ~ConvexHullShape();

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;

    /**
     * Creates anew ConvexHullShape from a Lua table.
     * @param table Lua table containing the parameters of this shape.
     * @return The new CuboidShape.
     */
    static std::unique_ptr<ConvexHullShape> luaGetFromTable(LuaTable& table);
private:
    /** Bullet shape. */
    btConvexHullShape shape;
};

#endif /* CONVEXHULLSHAPE_HPP */
