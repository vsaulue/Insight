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

#include "ConvexHullShape.hpp"
#include "ConvexMesh.hpp"
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaNativeString.hpp"

ConvexHullShape::ConvexHullShape(btScalar mass, const std::vector<btVector3>& vertices) :
    Shape(mass),
    shape(reinterpret_cast<const btScalar*>(vertices.data()), vertices.size())
{
    // TODO: proper center of mass & inertia matrix computation.
    shape.optimizeConvexHull();
}

ConvexHullShape::~ConvexHullShape() = default;

btCollisionShape& ConvexHullShape::getBulletShape() {
    return shape;
}

const btCollisionShape& ConvexHullShape::getBulletShape() const {
    return shape;
}

void ConvexHullShape::draw(ShapeDrawer& drawer, const btTransform& transform) const {
    std::vector<btVector3> vertices(shape.getNumVertices());
    for (int index = 0; index < shape.getNumVertices(); index++) {
        shape.getVertex(index, vertices[index]);
    }
    ConvexMesh mesh(std::move(vertices));
    mesh.addMargin(shape.getMargin());
    drawer.drawMesh(transform, mesh);
}

std::unique_ptr<ConvexHullShape> ConvexHullShape::luaGetFromTable(LuaTable& table) {
    btScalar mass = table.get<LuaNativeString,btScalar>("mass");
    LuaTable verticesTable = table.get<LuaNativeString,LuaTable>("vertices");
    std::vector<btVector3> vertices;
    for (unsigned i = 1; verticesTable.has<float>(i); i++) {
        vertices.push_back(verticesTable.get<float,btVector3>(i));
    }
    return std::make_unique<ConvexHullShape>(mass, vertices);
}
