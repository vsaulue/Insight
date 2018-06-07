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

#ifndef MESHTRIANGLE_HPP
#define MESHTRIANGLE_HPP

#include <array>
#include <functional>

#include "btBulletDynamicsCommon.h"

/** Triangle of a mesh from the physics engine. */
class MeshTriangle {
public:
    /**
     * Creates a new triangle.
     *
     * @param v1 First vertex of this triangle.
     * @param v2 Second vertex of this triangle.
     * @param v3 Third vertex of this triangle.
     */
    MeshTriangle(const btVector3& v1, const btVector3& v2, const btVector3& v3) :
        vertices{v1,v2,v3}
    {
        normal = (v2-v1).cross(v3-v1).normalize();
        offset = -normal.dot(v1);
    }

    /**
     * Creates a new triangle.
     *
     * @param v1 First vertex of this triangle.
     * @param v2 Second vertex of this triangle.
     * @param v3 Third vertex of this triangle.
     * @param Vertex which should return a negative offset by the plane equation.
     */
    MeshTriangle(const btVector3& v1, const btVector3& v2, const btVector3& v3, const btVector3& negativeVertex) :
        MeshTriangle(v1,v2,v3)
    {
        if (offsetOff(negativeVertex) > 0) {
            normal = -normal;
            offset = -offset;
            std::swap(vertices[1],vertices[2]);
        }
    }

    /**
     * Computes the value of the plane equation for the given point.
     * @param point Point to evaluate.
     * @return offset (signed distance) of this point to the plane.
     */
    btScalar offsetOff(const btVector3& point) const {
        return normal.dot(point) + offset;
    }

    /**
     * Tests if this triangle has the given point as a vertex.
     *
     * This is a test by address, not by value.
     *
     * @param point Point to check.
     * @return True if one of the 3 vertices of this triangle is the argument.
     */
    bool hasVertex(const btVector3& point) const {
        auto it = std::find_if(vertices.begin(), vertices.end(), [&](const btVector3& value) -> bool {
            return &point == &value;
        });
        return it != vertices.end();
    }

    /**
     * Gets a normal vector of this plane (normalized: length==1).
     * @return A normal vector of this plane.
     */
    const btVector3& getNormal() const {
        return normal;
    }

    /**
     * Gets an array containing the 3 vertices of this triangle.
     * @return The 3 vertices of this triangle.
     */
    const std::array<std::reference_wrapper<const btVector3>,3>& getVertices() const {
        return vertices;
    }
private:
    /** The 3 vertices of this triangle. */
    std::array<std::reference_wrapper<const btVector3>,3> vertices;
    /**
     * Offset of the plane defined by the vertices of this triangle.
     *
     * <code>d</code> coefficient of the plane equation <code>a*x + b*y + c*z + d = 0</code>
     */
    btScalar offset;
    /**
     * Normal vector of the plane defined by the vertices of this triangle.
     *
     * <code>a,b,c</code> coefficients of the plane equation <code>a*x + b*y + c*z + d = 0</code>
     */
    btVector3 normal;
};

#endif /* MESHTRIANGLE_HPP */
