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

#ifndef BASICMINIMUMSPANNINGTREE_HPP
#define BASICMINIMUMSPANNINGTREE_HPP

#include "BasicUndirectedGraph.hpp"

/** Computes a minimum spanning tree of an BasicUndirectedGraph.*/
class BasicMinimumSpanningTree {
public:
    using VertexIndex = BasicUndirectedGraph::VertexIndex;

    /**
     * Builds a minimum spanning tree for a graph.
     *
     * @param graph Graph on which the tree is generated.
     * @param root Root node of the spanning tree.
     */
    BasicMinimumSpanningTree(const BasicUndirectedGraph& graph, VertexIndex root);

    /**
     * Checks if the tree cover all the vertices in the graph.
     * @return True if the graph is connected.
     */
    bool isGraphConnected() const;

    /**
     * Checks if the spanning tree of the current connected component is unique.
     * @return True if the parsed connected component does not contain any cycle.
     */
    bool isUnique() const;

    /**
     * Iterates over each node of the spanning tree in a Depth-First order.
     * @param operation Operation to do on each node.
     * @tparam Callable object that can be called as void(VertexIndex).
     */
    template<typename Callable>
    void depthFirstForEach(Callable operation) const {
        depthFirstForEach(operation, rootIndex);
    }

    /**
     * Returns the data structure containing the tree.
     * @return A map associating a vertex index to the indices of its child in the tree.
     */
    const std::unordered_map<VertexIndex,std::unordered_set<VertexIndex>>& getTree() const {
        return tree;
    }
private:
    /** Graph parsed by this algorithm. */
    const BasicUndirectedGraph& graph;
    /** Index of the root vertex of the tree. */
    VertexIndex rootIndex;
    /** Tree nodes. */
    std::unordered_map<VertexIndex,std::unordered_set<VertexIndex>> tree;
    /** True if no cycles were detected in the parsed connected component. */
    bool unique;

    /**
     * Iterates over each node of a subtree in a Depth-First order.
     * @param operation Operation to do on each node.
     * @param node Starting node in the tree.
     * @tparam Callable object that can be called as void(VertexIndex).
     */
    template<typename Callable>
    void depthFirstForEach(Callable operation, VertexIndex node) const {
        operation(node);
        for (VertexIndex childNode : tree.at(node)) {
            depthFirstForEach(operation, childNode);
        }
    }

    /**
     * Builds the tree recursively.
     * @param current Index of the node to build.
     * @param origin Parent of this node.
     */
    void buildTree(VertexIndex current, VertexIndex origin);
};

#endif /* BASICMINIMUMSPANNINGTREE_HPP */
