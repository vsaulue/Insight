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

#ifndef CONVEXMESH_HPP
#define CONVEXMESH_HPP

#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

#include "btBulletDynamicsCommon.h"

#include "MeshTriangle.hpp"

/** Convex mesh from the physics engine. */
class ConvexMesh {
public:
    /**
     * Creates a new Mesh from the vertices of a convex hull (copy constructor).
     * @param vertices Vertices of the convex hull.
     */
    ConvexMesh(const std::vector<btVector3>& vertices);

    /**
     * Creates a new Mesh from the vertices of a convex hull (move constructor).
     * @param vertices Vertices of the convex hull.
     */
    ConvexMesh(std::vector<btVector3>&& vertices);

    /**
     * Extrude all faces along their normals on the given distance.
     *
     * Only positive margins are supported.
     *
     * @param margin Extrusion distance.
     */
    void addMargin(btScalar margin);

    /**
     * Gets the set of triangles of this mesh.
     *
     * <ul>
     * <li>The normal of a triangle is directed to the outside of the mesh.</li>
     * <li>Seen from outside the mesh, vertices of a triangle are stored in CCW order (counter clockwise).</li>
     * </ul>
     *
     * @return The set of triangles of this mesh.
     */
    const std::unordered_set<std::unique_ptr<MeshTriangle>>& getTriangles() const {
        return triangles;
    }
private:
    /** List of vertices. */
    std::vector<btVector3> vertices;
    /** List of triangles (see getTriangles() for constraints on its content). */
    std::unordered_set<std::unique_ptr<MeshTriangle>> triangles;
    /** An arbitrary point which is strictly inside the convex mesh. */
    btVector3 center;

    /**
     * Adds a new vertex to the current convex mesh.
     *
     * Used to build incrementally a mesh from a convex hull.
     *
     * @param vertex Vertex to add.
     */
    void addVertex(const btVector3& vertex);
};

#endif /* CONVEXMESH_HPP */
