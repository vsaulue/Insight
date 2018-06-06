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

#include <array>
#include <limits>

#include "ConvexMesh.hpp"

/**
 * Finds the element miximizing a given function in the range [first, last].
 *
 * @param first Iterator to the first item of the range to examine.
 * @param last Iterator to the item past the last element of the range to examine.
 * @param function Function to maximize. Must be callable with a <code>const Iterator::value_type&</code> argument,
 * and returns a <code>btScalar</code>.
 * @return An iterator the the item that maximized the given function.
 *
 * @tparam Iterator Type of the iterator used to access the range of elements.
 * @tparam Operation Type of the function to maximize.
 */
template<typename Iterator, typename Operation>
Iterator maximize_element(Iterator first, Iterator last, Operation function) {
    Iterator result = last;
    btScalar max = -std::numeric_limits<btScalar>::infinity();
    for (Iterator it = first; it != last; it++) {
        btScalar current = function(*it);
        if (current > max) {
            max = current;
            result = it;
        }
    }
    return result;
}

/** Edge of a ConvexMesh. */
class MeshEdge {
public:
    /**
     * Creates a new edge.
     *
     * The vertices might be reordered internally.
     *
     * @param v1 First vertex of the edge.
     * @param v2 Second vertex of the edge;
     */
    MeshEdge(const btVector3& v1, const btVector3& v2) :
        vertices{v1,v2}
    {
        if (&v1 > &v2) {
            std::swap(vertices[0],vertices[1]);
        }
    }

    /**
     * Compares the address of the stored vertices (NOT a value comparison).
     *
     * @param other Other edge to compare to this object.
     * @return The result of the comparison.
     */
    bool operator==(const MeshEdge& other) const {
        bool result = true;
        for (unsigned i = 0; i < vertices.size(); i++) {
            result = result && (&other.vertices[i].get() == &vertices[i].get());
        }
        return result;
    }

    /**
     * Gets the first vertex of this edge.
     * @return The first vertex of this edge.
     */
    const btVector3& first() const {
        return vertices[0];
    }

    /**
     * Gets the second vertex of this edge.
     * @return The second vertex of this edge.
     */
    const btVector3& second() const {
        return vertices[1];
    }

    /** Custom hash implementation. */
    struct Hash {
        /**
         * Hash the pointers (NOT the values) of the vertices of this edge
         *
         * @param object Edge to hash.
         * @return The computed hash value.
         */
        std::size_t operator()(const MeshEdge& object) const {
            std::size_t result = 0;
            for (const btVector3& index : object.vertices) {
                result ^= std::hash<const btVector3*>()(&index);
            }
            return result;
        }
    };
private:
    /** The 2 vertices of this edge. */
    std::array<std::reference_wrapper<const btVector3>, 2> vertices;
};

ConvexMesh::ConvexMesh(std::vector<btVector3>&& inputVertices) :
    vertices(inputVertices),
    center(0,0,0)
{
    if (inputVertices.size() < 4) {
        throw std::invalid_argument("A convex hull requires at least 4 vertices.");
    }
    std::vector<std::reference_wrapper<const btVector3>> firstVertices(4, *vertices.begin());

    // first 2 vertices: min & max X coordinate
    auto pair = std::minmax_element(vertices.begin(), vertices.end(), [](const btVector3& a, const btVector3& b) -> bool {
        return a.x() < b.x();
    });
    firstVertices[0] = *pair.first;
    firstVertices[1] = *pair.second;

    // 3rd vertex: farthest vertex from the line defined by the first 2 vertices.
    const btVector3 direction = (firstVertices[1] - firstVertices[0]).normalize();
    firstVertices[2] = *maximize_element(vertices.begin(), vertices.end(), [&](const btVector3& value) -> btScalar {
        const btVector3 delta = value - firstVertices[0];
        return (delta-direction.dot(delta)*direction).length2();
    });

    // 4th vertex: farthest vertex from the plane defined by the first 3 vertices.
    MeshTriangle firstTriangle(firstVertices[0], firstVertices[1], firstVertices[2]);
    firstVertices[3] = *maximize_element(vertices.begin(), vertices.end(), [&](const btVector3& value) -> btScalar {
        return btFabs(firstTriangle.offsetOff(value));
    });

    for (auto& vertex : firstVertices) {
        center+= vertex;
    }
    center /= firstVertices.size();
    for (unsigned i = 0; i < 4; i++) {
        triangles.insert(std::make_unique<MeshTriangle>(firstVertices[i], firstVertices[(i+1)%4], firstVertices[(i+2)%4], center));
    }

    for (auto& vertex: vertices) {
        auto it = std::find_if(firstVertices.begin(), firstVertices.end(), [&vertex](const btVector3& value) -> bool {
            return &value == &vertex;
        });
        if (it == firstVertices.end()) {
            addVertex(vertex);
        }
    }
}

ConvexMesh::ConvexMesh(const std::vector<btVector3>& vertices) :
    ConvexMesh(std::vector(vertices))
{

}

void ConvexMesh::addVertex(const btVector3& vertex) {
    std::unordered_set<MeshEdge,MeshEdge::Hash> horizon;
    for (auto it = triangles.begin(); it != triangles.end(); /*empty*/) {
        const MeshTriangle& triangle = **it;
        if (triangle.offsetOff(vertex) > 0) {
            const auto& vertices = triangle.getVertices();
            std::array<MeshEdge,3> edges = {{
                {vertices[0], vertices[1]},
                {vertices[0], vertices[2]},
                {vertices[1], vertices[2]},
            }};
            for (const MeshEdge& edge : edges) {
                auto it = horizon.find(edge);
                if (it == horizon.end()) {
                    horizon.insert(edge);
                } else {
                    horizon.erase(it);
                }
            }
            it = triangles.erase(it);
        } else {
            it++;
        }
    }
    for (const MeshEdge& edge : horizon) {
        triangles.insert(std::make_unique<MeshTriangle>(vertex, edge.first(), edge.second(), center));
    }
}