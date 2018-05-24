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

#ifndef BASICUNDIRECTEDGRAPH_HPP
#define BASICUNDIRECTEDGRAPH_HPP

#include <cstddef>
#include <unordered_map>
#include <unordered_set>

/** basic graph, with vertex indexed by integers. */
class BasicUndirectedGraph {
public:
    /** Type used to represent a vertex index in the graph. */
    using VertexIndex = unsigned;

    /** Vertex in a graph. */
    class Vertex {
    public:
        /**
         * Creates a new Vertex.
         * @param graph Graph containing the vertex.
         * @param index Index of the vertex.
         */
        Vertex(const BasicUndirectedGraph& graph, VertexIndex index) :
            graph(graph),
            index(index)
        {

        }

        /**
         * Gets the index of this vertex.
         * @return The index of this vertex.
         */
        VertexIndex getIndex() {
            return index;
        }

        /**
         * Gets the set of vertex indices adjacents to the current vertex.
         * @return The set of neighbours' index.
         */
        const std::unordered_set<VertexIndex>& getNeighbours() {
            return graph.adjacencyList.at(index);
        }
    private:
        /** Graph containing this vertex. */
        const BasicUndirectedGraph& graph;
        /** Index of this vertex. */
        VertexIndex index;
    };

    /** Creates a new empty graph. */
    BasicUndirectedGraph();

    /**
     * Creates a new vertex.
     * @return The index of the new vertex.
     */
    VertexIndex newVertex();

    /**
     * Adds a new edge to the graph.
     *
     * The edge should not be already present.
     *
     * @param a A valid vertex index.
     * @param b A valid vertex index.
     */
    void addEdge(VertexIndex a, VertexIndex b);

    /**
     * Wraps a VertexIndex into a Vertex object.
     *
     * This can be used to access the list of adjacent vertices of a given vertex.
     *
     * @param index Index of the vertex.
     * @return A wrapper of the given index.
     */
    Vertex getVertex(VertexIndex index) const {
        return Vertex(*this, index);
    }

    /**
     * Counts the number of vertices in this graph.
     * @return The number of vertices in the graph.
     */
    std::size_t countVertices() const {
        return adjacencyList.size();
    }
private:
    /** Maximum vertex index in the graph. */
    VertexIndex currentMaxIndex;
    /** Map giving the list of adjacent vertices for a given vertex index. */
    std::unordered_map<VertexIndex,std::unordered_set<VertexIndex>> adjacencyList;
};

#endif /* BASICUNDIRECTEDGRAPH_HPP */
