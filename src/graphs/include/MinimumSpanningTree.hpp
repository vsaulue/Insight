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

#ifndef MINIMUMSPANNINGTREE_HPP
#define MINIMUMSPANNINGTREE_HPP

#include "BasicMinimumSpanningTree.hpp"
#include "BasicUndirectedGraph.hpp"
#include "UndirectedGraph.hpp"

/** Computes a minimum spanning tree of an UndirectedGraph.*/
template<typename VertexKeyType, typename EdgeKeyType>
class MinimumSpanningTree {
public:
    /** Graph type handled by this spanning tree algorithm. */
    using Graph = UndirectedGraph<VertexKeyType, EdgeKeyType>;
    /** Vertex type handled by this spanning tree algorithm. */
    using Vertex = typename Graph::Vertex;
    /** VertexIndex of BasicUndirectedGraph. */
    using VertexIndex = BasicUndirectedGraph::VertexIndex;

    /**
     * Builds a minimum spanning tree for a graph.
     *
     * @param graph Graph on which the tree is generated.
     * @param root Key of the root node of the spanning tree.
     */
    MinimumSpanningTree(const Graph& graph, const VertexKeyType& root) :
        graph(graph),
        root(graph.getVertex(root)),
        basicTree(graph.getBasicGraph(), graph.getVertexIndex(root))
    {

    }

    /**
     * Checks if the tree cover all the vertices in the graph.
     * @return True if the graph is connected.
     */

    bool isGraphConnected() const {
        return basicTree.isGraphConnected();
    }

    /**
     * Checks if the spanning tree of the current connected component is unique.
     * @return True if the parsed connected component does not contain any cycle.
     */
    bool isUnique() const {
        return basicTree.isUnique();
    }

    /**
     * Iterates over each node of the spanning tree in a Depth-First order.
     *
     * Passes a single argument to the operation function.
     * <ul>
     * <li>first: pointer to the key of the current vertex.</li>
     * <li>second: pointer to the key of the edge linking this vertex to its parent in the graph (nullptr for the root of the tree).</li>
     * </ul>
     *
     * @param operation Operation to do on each node.
     * @tparam Callable object that can be called as void(VertexIndex).
     */
    template<typename Callable>
    void depthFirstForEach(Callable operation) const {
        depthFirstForEachImpl(operation, nullptr, root);
    }
private:
    /** Graph on which the spanning tree was computed. */
    const Graph& graph;
    /** Root of the computed spanning tree. */
    Vertex root;
    /** Algorithm executed on the underlying BasicMinimumSpanningTree. */
    BasicMinimumSpanningTree basicTree;

    /**
     * Recursive implementation of depthFirstForEach.
     *
     * @param operation Operation to do on each node.
     * @param source Parent node in the tree (nullptr for the root).
     * @param current Current node being traversed.
     *
     * @tparam Callable object that can be called as void(VertexIndex).
     */
    template<typename Callable>
    void depthFirstForEachImpl(Callable operation, const Vertex* source, const Vertex& current) const {
        std::pair<const VertexKeyType*, const EdgeKeyType*> pair(&current.getkey(), nullptr);
        if (source != nullptr) {
            pair.second = &graph.getEdgeByIndexes(source->getIndex(), current.getIndex());
        }
        operation(pair);
        for (VertexIndex neighbourIndex : basicTree.getTree().at(current.getIndex())) {
            Vertex neighbour = graph.getVertexByIndex(neighbourIndex);
            depthFirstForEachImpl(operation, &current, neighbour);
        }
    }
};

#endif /* MINIMUMSPANNINGTREE_HPP */
