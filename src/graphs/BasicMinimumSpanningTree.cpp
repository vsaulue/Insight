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

#include "BasicMinimumSpanningTree.hpp"

BasicMinimumSpanningTree::BasicMinimumSpanningTree(const BasicUndirectedGraph& graph, VertexIndex root) :
    graph(graph)
{
    rootIndex = root;
    unique = true;
    buildTree(root, root);
}

bool BasicMinimumSpanningTree::isUnique() const {
    return unique;
}

bool BasicMinimumSpanningTree::isGraphConnected() const {
    return tree.size() == graph.countVertices();
}

void BasicMinimumSpanningTree::buildTree(VertexIndex current, VertexIndex origin) {
    std::unordered_set<VertexIndex>& node = tree[current];
    for (VertexIndex neighbour : graph.getVertex(current).getNeighbours()) {
        if (neighbour == origin) {
            continue;
        } else if (tree.find(neighbour) == tree.end()) {
            node.insert(neighbour);
            buildTree(neighbour, current);
        } else {
            unique = false;
        }
    }
}
