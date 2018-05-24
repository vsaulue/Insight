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

#include <exception>
#include <string>

#include "BasicUndirectedGraph.hpp"

BasicUndirectedGraph::BasicUndirectedGraph() {
    currentMaxIndex = 0;
}

BasicUndirectedGraph::VertexIndex BasicUndirectedGraph::newVertex() {
    currentMaxIndex++;
    adjacencyList[currentMaxIndex] = {};
    return currentMaxIndex;
}

void BasicUndirectedGraph::addEdge(VertexIndex a, VertexIndex b) {
    auto checkVertex = [this](VertexIndex v) {
        if (this->adjacencyList.find(v) == this->adjacencyList.end()) {
            std::string msg = std::string("Invalid vertex index: " + std::to_string(v)) + ".";
            throw std::out_of_range(msg);
        }
    };
    checkVertex(a);
    checkVertex(b);

    const auto& setA = adjacencyList.at(a);
    if (setA.find(b) != setA.end()) {
        throw std::domain_error("This edge already exists.");
    }

    adjacencyList[a].insert(b);
    adjacencyList[b].insert(a);
}
