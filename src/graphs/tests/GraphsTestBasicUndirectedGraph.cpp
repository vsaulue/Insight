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
#include <exception>
#include <vector>

#include <catch.hpp>

#include "BasicUndirectedGraph.hpp"

TEST_CASE("BasicUndirectedGraph") {
    const int VERTICES_COUNT = 5;
    using Index = BasicUndirectedGraph::VertexIndex;
    using Vertex = BasicUndirectedGraph::Vertex;
    BasicUndirectedGraph graph;
    SECTION("Add vertices") {
        std::array<Index,VERTICES_COUNT> indices;
        for (Index& i : indices) {
            i = graph.newVertex();
        }

        REQUIRE(graph.countVertices() == VERTICES_COUNT);

        std::vector<Vertex> vertices;
        for (Index i : indices) {
            vertices.push_back(graph.getVertex(i));
        }

        REQUIRE(graph.countVertices() == VERTICES_COUNT);

        SECTION("0 edge case") {
            for (Vertex& v : vertices) {
                REQUIRE(v.getNeighbours().size() == 0);
            }
        }

        SECTION("Add valid edges") {
            const int EDGES_COUNT = 4;
            std::array<std::pair<Index,Index>, EDGES_COUNT> edges = {{
                {indices[1],indices[4]},
                {indices[2],indices[3]}, {indices[2],indices[4]},
                {indices[4],indices[3]},
            }};
            for (auto& edge : edges) {
                graph.addEdge(edge.first, edge.second);
            }

            int countEdge2 = 0;
            for (Vertex& v1 : vertices) {
                Index i1 = v1.getIndex();
                for (Index i2 : v1.getNeighbours()) {
                    countEdge2++;
                    auto iterator = std::find_if(edges.begin(),edges.end(),[i1,i2](auto& pair)->bool {
                        return (pair == std::make_pair(i1,i2)) || (pair == std::make_pair(i2,i1));
                    });
                    REQUIRE(iterator != edges.end());
                }
            }
            REQUIRE(countEdge2/2 == edges.size());

            SECTION("Re-add existing edge (must throw)") {
                bool error = false;
                Index badIndex = 1+*std::max_element(indices.begin(), indices.end());
                try {
                    graph.addEdge(badIndex, indices[0]);
                } catch (const std::exception&) {
                    error = true;
                }
                REQUIRE(error);
            }
        }
    }
}