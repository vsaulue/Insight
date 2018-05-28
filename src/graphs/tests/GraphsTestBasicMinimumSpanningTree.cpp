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

#include <catch.hpp>

#include "BasicMinimumSpanningTree.hpp"
#include "BasicUndirectedGraph.hpp"
#include "GraphsTestCommon.hpp"

using Index = BasicUndirectedGraph::VertexIndex;

/** Number of vertices in graph A. */
static constexpr unsigned GRAPH_A_COUNT = 15;

/**
 * Initialise graph A.
 *
 * @param graph Graph to initialise.
 * @param indices Mapping: unsigned to Index.
 */
static void initGraphA(BasicUndirectedGraph& graph, std::array<Index,GRAPH_A_COUNT>& indices) {
    for (auto& i : indices) {
        i = graph.newVertex();
    }

    std::array<std::pair<unsigned,unsigned>, 14> edges = {{
        {0,5},{0,7},{0,14},{0,6},
        {5,1},
        {1,2},{1,3},
        {7,4},{7,13},{7,10},
        {10,9},{10,8},{10,12},
        {12,11},
    }};
    for (auto& edge : edges) {
        graph.addEdge(indices[edge.first], indices[edge.second]);
    }
}

/** Number of vertices in graph B. */
static constexpr unsigned GRAPH_B_COUNT = 8;

/**
 * Initialise graph B.
 *
 * @param graph Graph to initialise.
 * @param indices Mapping: unsigned to Index.
 */
static void initGraphB(BasicUndirectedGraph& graph, std::array<Index, GRAPH_B_COUNT>& indices) {
    for (auto& i : indices) {
        i = graph.newVertex();
    }

    std::array<std::pair<unsigned,unsigned>, 7> edges = {{
        {0,1},{1,2},{2,3},{3,0},
        {5,4},{5,6},{5,7},
    }};
    for (auto& edge : edges) {
        graph.addEdge(indices[edge.first], indices[edge.second]);
    }
}

TEST_CASE("BasicMinimumSpanningTree") {
    SECTION("Connected, acyclic graph test.") {
        // Acyclic, connected graph.
        std::array<Index, GRAPH_A_COUNT> indicesA;
        BasicUndirectedGraph graphA;
        initGraphA(graphA, indicesA);

        BasicMinimumSpanningTree tree(graphA, indicesA[7]);

        SECTION("Check unique.") {
            REQUIRE(tree.isUnique());
        }

        SECTION("Check connected.") {
            REQUIRE(tree.isGraphConnected());
        }

        SECTION("Check Depth-first order.") {
            std::vector<Index> result;
            tree.depthFirstForEach([&result](Index i) { result.push_back(i); });
            REQUIRE(result.size() == GRAPH_A_COUNT);

            std::array<unsigned,GRAPH_A_COUNT> parents = {{7,5,1,1,7,0,0,7,10,10,7,12,10,7,0}};
            for (unsigned u = 0; u < parents.size(); u++) {
                unsigned parentOrder = getIndex(result, indicesA[parents[u]]);
                unsigned currentOrder = getIndex(result, indicesA[u]);
                REQUIRE(parentOrder <= currentOrder);
            }
        }
    }

    SECTION("Unconnected graph.") {
        // Unconnected graph, one component with cycles, one without.
        std::array<Index, GRAPH_B_COUNT> indicesB;
        BasicUndirectedGraph graphB;
        initGraphB(graphB, indicesB);

        SECTION("Acyclic subgraph.") {
            const Index testedIndex = indicesB[6];
            BasicMinimumSpanningTree tree(graphB, testedIndex);

            SECTION("Check unique.") {
                REQUIRE(tree.isUnique());
            }

            SECTION("Check NOT connected.") {
                REQUIRE_FALSE(tree.isGraphConnected());
            }

            SECTION("Check Depth-first order") {
                std::vector<Index> result;
                tree.depthFirstForEach([&result](Index i) { result.push_back(i); });
                REQUIRE(result.size() == 4);
                REQUIRE(result[0] == testedIndex);
                REQUIRE(result[1] == indicesB[5]);
            }
        }

        SECTION("Subgraph with cycle.") {
            const Index testedIndex = indicesB[2];
            BasicMinimumSpanningTree tree(graphB, testedIndex);

            SECTION("Check NOT unique.") {
                REQUIRE_FALSE(tree.isUnique());
            }

            SECTION("Check NOT connected.") {
                REQUIRE_FALSE(tree.isGraphConnected());
            }

            SECTION("Check Depth-first order") {
                std::vector<Index> result;
                tree.depthFirstForEach([&result](Index i) { result.push_back(i); });
                REQUIRE(result.size() == 4);
                REQUIRE(result[0] == testedIndex);
            }
        }
    }
}
