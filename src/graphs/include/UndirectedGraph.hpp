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

#ifndef UNDIRECTEDGRAPH_HPP
#define UNDIRECTEDGRAPH_HPP

#include <unordered_set>

#include "boost/bimap.hpp"
#include "boost/bimap/unordered_set_of.hpp"

#include "BasicUndirectedGraph.hpp"

/**
 * Represents an undirected graph.
 *
 * It is possible to associate a unique key/label to each vertex and each edge.
 *
 * @tparam VertexKeyType Type of the key/label for vertices.
 * @tparam EdgeKeyType Type of the key/lbal for edges.
 */
template<typename VertexKeyType, typename EdgeKeyType>
class UndirectedGraph {
private:
    /** Index used in BasicUndirectedGraph. */
    using VertexIndex = BasicUndirectedGraph::VertexIndex;
    /** Index of an edge. */
    using EdgeIndex = std::pair<VertexIndex,VertexIndex>;
public:
    class Vertex {
    public:
        using BasicVertex = BasicUndirectedGraph::Vertex;

        /**
         * Constructs a new Vertex.
         * @param graph Graph holding this vertex.
         * @param key Key of the vertex in the graph.
         */
        Vertex(const UndirectedGraph& graph, const VertexKeyType& key) :
            graph(graph),
            key(key),
            basicIndex(graph.keyToVertex.left.at(key)),
            basicNeighbours(graph.basicGraph.getVertex(basicIndex).getNeighbours()),
            neighbours(*this)
        {

        }

        /**
         * Get the key of this vertex.
         * @return The key of this vertex.
         */
        const VertexKeyType& getkey() const {
            return key;
        }

        /** Class used to interact with the neighbours of a given vertex. */
        class Neighbours {
        private:
            /** Vertex whose neighbours are represented by this object.*/
            const Vertex& vertex;
        public:
            /**
             * Constructs a new neighbour object.
             * @param vertex Vertex holding this object.
             */
            Neighbours(const Vertex& vertex) :
                vertex(vertex)
            {

            }

            Neighbours(const Neighbours&) = delete;

            /**
             * Constant iterator for Neighbours object.
             *
             * This iterator will point to a pair<const VertexKeyType*, const EdgeKeyType*>.
             * This value is held in temporary storage: any reference or pointer to it will
             * become invalid when the iterator is modified or deleted.
             */
            class const_iterator {
            private:
                /** Iterator on the neighbour's indexes in the underlying BasicUndirectedGraph. */
                using basic_iterator = std::unordered_set<VertexIndex>::const_iterator;
            public:
                using value_type = std::pair<const VertexKeyType*,const EdgeKeyType*>;
                using reference = const value_type&;
                using pointer = const value_type*;

                /**
                 * Iterator constructor.
                 * @param vertex Vertex whose neighbours will be iterated on.
                 * @param iterator Iterator to the set of adjacent indexes in the basic graph.
                 */
                const_iterator(const Vertex& vertex, const basic_iterator& iterator) :
                    vertex(vertex),
                    basicIterator(iterator),
                    upToDate(false),
                    value(nullptr,nullptr)
                {

                }

                /**
                 * Hop to the next value.
                 *
                 * Invalidates the last pair returned by operator* or operator->.
                 */
                void operator++() {
                    upToDate = false;
                    basicIterator++;
                }

                /** Iterator inequality test. */
                bool operator!=(const const_iterator& other) {
                    return basicIterator != other.basicIterator;
                }

                /** Iterator equality test. */
                bool operator==(const const_iterator& other) {
                    return basicIterator == other.basicIterator;
                }

                /**
                 * Returns a reference to the current value of this iterator.
                 * @return A temporary reference to the current value of this iterator.
                 */
                reference operator*() {
                    updateValue();
                    return value;
                }

                /**
                 * Returns a pointer to the current value of this iterator.
                 * @return A pointer to the current value of this iterator.
                 */
                pointer operator->() {
                    updateValue();
                    return &value;
                }
            private:
                /**
                 * Update the value field of this iterator.
                 */
                void updateValue() {
                    if(!upToDate) {
                        VertexIndex i1 = *basicIterator;
                        VertexIndex i2 = vertex.basicIndex;
                        value.first = &vertex.graph.keyToVertex.right.at(i1);

                        EdgeIndex basicPair(std::min(i1,i2),std::max(i1,i2));
                        value.second = &vertex.graph.keyToEdge.right.at(basicPair);
                        upToDate = true;
                    }
                }

                /** Vertex whose neighbour are being iterated. */
                const Vertex& vertex;
                /** Iterator on the indexes of adjacent vertices. */
                basic_iterator basicIterator;
                /** Flag set when the value field has valid content. */
                bool upToDate;
                /** Value of the current item of the iteration. */
                value_type value;
            };

            /**
             * Iterator to the beginning of the set of neighbours.
             *
             * iterates over values of type pair<const VertexKeyType*,const EdgeKeyType*>,
             * containing an adjacent vertex key, and the key of the edge connecting it
             * to this vertex.
             *
             * These pair are held in temporary storage. They become invalid when the
             * iterator is incremented or deleted.
             *
             * @return Iterator to the first adjacent vertice.
             */
            const_iterator begin() const {
                return const_iterator(vertex, vertex.basicNeighbours.begin());
            }

            /**
             * Iterator to the end of the set of neighbours.
             * @return Iterator to the end of the set of adjacent vertices.
             */
            const_iterator end() const {
                return const_iterator(vertex, vertex.basicNeighbours.end());
            }

            /**
             * Gets the number of adjacent vertices.
             * @return The number of neighbour vertices.
             */
            std::size_t size() const {
                return vertex.basicNeighbours.size();
            }
        };

        /**
         * Gets an object to read adjacent vertices.
         * @return The neighbours of the current vertex.
         */
        const Neighbours& getNeighbours() {
            return neighbours;
        }
    private:
        /** Graph holding the vertex and its neighbours. */
        const UndirectedGraph& graph;
        /** Primary key of this vertex. */
        const VertexKeyType& key;
        /** Index of this vertex. */
        VertexIndex basicIndex;
        /** List of adjacent vertices to this vertex in the underlying BasicUndirectedGraph. */
        const std::unordered_set<VertexIndex>& basicNeighbours;
        /** Proxy object to read the neighbours if this vertex. */
        Neighbours neighbours;
    };

    /**
     * Adds a new vertex into this graph.
     * @param vertexKey Key of the new vertex.
     */
    void newVertex(const VertexKeyType& vertexKey) {
        if (keyToVertex.left.find(vertexKey) != keyToVertex.left.end()) {
            throw std::invalid_argument("This vertex already exists in the graph.");
        }
        VertexIndex index = basicGraph.newVertex();
        keyToVertex.left.insert(std::make_pair(vertexKey,index));
    }

    /**
     * Adds a new edge into this graph.
     *
     * @param edgeKey Primary key associated to this edge.
     * @param vertexA Key of the first vertex of this edge.
     * @param vertexB Key of the second vertex of this edge.
     */
    void addEdge(const EdgeKeyType& edgeKey, const VertexKeyType& vertexA, const VertexKeyType& vertexB) {
        if(keyToEdge.left.find(edgeKey) != keyToEdge.left.end()) {
            throw std::invalid_argument("This edge already exists in the graph.");
        }
        VertexIndex a = getVertexIndex(vertexA);
        VertexIndex b = getVertexIndex(vertexB);
        basicGraph.addEdge(a, b);
        EdgeIndex edge(std::min(a,b),std::max(a,b));
        keyToEdge.left.insert(std::make_pair(edgeKey,edge));
    }

    /**
     * Gets an object representing a vertex of this graph.
     *
     * This object can be used for more advances operation on a vertex (like accessing
     * all its neighbours).
     *
     * @param key Key of the vertex in this graph.
     * @return A vertex object associated to the given key.
     */
    Vertex getVertex(const VertexKeyType& key) const {
        return Vertex(*this, key);
    }

    /**
     * Gets the total number of vertices in this graph.
     * @return The total number of vertices in this graph.
     */
    std::size_t countVertices() const {
        return basicGraph.countVertices();
    }
private:
    /**
     * Boost bidirectional map alias.
     *
     * @tparam T1 Left type of the bimap.
     * @tparam T2 Right type of the bimap.
     */
    template<typename T1, typename T2>
    using bimap = boost::bimap<boost::bimaps::unordered_set_of<T1>,boost::bimaps::unordered_set_of<T2>>;

    /** Bidirectional map: vertex keys <-> vertex indexes. */
    bimap<VertexKeyType,VertexIndex> keyToVertex;
    /** Bidirectional map: edge keys <-> edge indexes. */
    bimap<EdgeKeyType, EdgeIndex> keyToEdge;

    /** Basic graph: representation of this graph using integer indexes. */
    BasicUndirectedGraph basicGraph;

    VertexIndex getVertexIndex(const VertexKeyType& vertex) const {
        auto it = this->keyToVertex.left.find(vertex);
        if (it == this->keyToVertex.left.end()) {
            throw std::invalid_argument("Vertex does not exist.");
        }
        return it->second;
    }
};

#endif /* UNDIRECTEDGRAPH_HPP */
