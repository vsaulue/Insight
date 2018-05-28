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

#include <catch.hpp>

#include "GraphsTestCommon.hpp"
#include "MinimumSpanningTree.hpp"
#include "UndirectedGraph.hpp"

/** Vertices of graph A (acyclic, connected). */
static std::vector<std::string> verticesA = {
    "head","torso",
    "leftThigh","leftLeg","leftFoot","leftToes",
    "rightThigh","rightLeg","rightFoot","rightToes",
    "leftHand","leftArm","leftForearm",
    "rightHand","rightArm","rightForearm",
};

/** Edges of graph A (acyclic, connected). */
static std::vector<std::tuple<std::string,std::string,std::string>> edgesA = {
    {"neck","torso","head"},
    {"leftHip","torso","leftThigh"}, {"rightHip","torso","rightThigh"},
    {"leftKnee","leftThigh","leftLeg"}, {"rightKnee","rightThigh","rightLeg"},
    {"leftAnkle","leftLeg","leftFoot"}, {"rightAnkle","rightLeg","rightFoot"},
    {"leftToeJoint","leftFoot","leftToes"}, {"rightToeJoint","rightFoot","rightToes"},
    {"leftWrist","leftHand","leftForearm"}, {"rightWrist","rightHand","rightForearm"},
    {"leftElbow","leftForearm","leftArm"}, {"rightElbow","rightForearm","rightArm"},
    {"leftShoulder","leftArm","torso"}, {"rightShoulder","rightArm","torso"},
};

/** Vertices of graph B (unconnected, with cycles). */
static std::vector<std::string> verticesB = {
    "A1","A2","A3","A4",
    "B1","B2","B3","B4",
};

/** Edges of graph B (unconnected, with cycles). */
static std::vector<std::tuple<std::string,std::string,std::string>> edgesB = {
    {"a12","A1","A2"},
    {"a23","A2","A3"},
    {"a34","A3","A4"},
    {"a31","A3","A1"},
    {"b12","B2","B1"},
    {"b13","B3","B1"},
    {"b14","B4","B1"},
};

/**
 * Initialise a graph.
 * @param graph Graph to initialise.
 * @param vertices Set of vertices to insert into the graph.
 * @param edges Set of edges to insert into the graph.
 */
static void initGraph(UndirectedGraph<std::string,std::string>& graph, const std::vector<std::string>& vertices,
                      const std::vector<std::tuple<std::string,std::string,std::string>>& edges)
{
    for (auto& vertex : vertices) {
        graph.newVertex(vertex);
    }
    for (auto& edge : edges) {
        graph.addEdge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
    }
}

/**
 * Checks that the specified edge exists.
 *
 * @param[in] edges Set of edge to llik in.
 * @param[in] name Name of the edge to look up.
 * @param[in] v1 Name of the first vertex of the edge.
 * @param[in] v2 Name of the second vertex of the edge.
 */
static void checkEdge(const std::vector<std::tuple<std::string,std::string,std::string>>& edges,
                        const std::string& name, const std::string& v1, const std::string& v2)
{
    auto it = std::find_if(edges.begin(), edges.end(), [&name](auto& tuple) -> bool {
        return (name == std::get<0>(tuple));
    });
    REQUIRE(it != edges.end());
    bool hasV1 = (v1 == std::get<1>(*it)) || (v1 == std::get<2>(*it));
    REQUIRE(hasV1);
    bool hasV2 = (v2 == std::get<1>(*it)) || (v2 == std::get<2>(*it));
    REQUIRE(hasV2);
}

// Checks depth first iteration on tree on a connected, acyclic component of a graph.
static void checkDepthFirstAcyclic(const std::vector<std::tuple<std::string,std::string,std::string>>& edges,
                                   const std::unordered_map<std::string,std::string>& parents, const MinimumSpanningTree<std::string,std::string>& tree,
                                   const std::string& root)
{
    std::vector<std::reference_wrapper<const std::string>> result;
    tree.depthFirstForEach([&](std::pair<const std::string*,const std::string*>& pair) {
        result.push_back(*pair.first);
        if (*pair.first != root) {
            REQUIRE(pair.second != nullptr);
            checkEdge(edges, *pair.second, *pair.first, parents.at(*pair.first));
        }
    });
    REQUIRE(result.size() == parents.size());
    for (const auto& pair : parents) {
        unsigned parentOrder = getIndex(result, pair.second);
        unsigned currentOrder = getIndex(result, pair.first);
        REQUIRE(parentOrder <= currentOrder);
    }
}

TEST_CASE("MinimumSpanningTree") {
    UndirectedGraph<std::string,std::string> graph;

    SECTION("Connected, acyclic graph.") {

        initGraph(graph, verticesA, edgesA);
        const std::string root("torso");
        MinimumSpanningTree<std::string,std::string> tree(graph, root);

        SECTION("Check unique") {
            REQUIRE(tree.isUnique());
        }

        SECTION("Check connected") {
            REQUIRE(tree.isGraphConnected());
        }

        SECTION("Check Depth First order") {
            const std::unordered_map<std::string,std::string> parents = {
                {"head","torso"},
                {"torso","torso"},
                {"leftHand","leftForearm"}, {"leftForearm","leftArm"}, {"leftArm","torso"},
                {"rightHand","rightForearm"}, {"rightForearm","rightArm"}, {"rightArm","torso"},
                {"leftToes","leftFoot"}, {"leftFoot","leftLeg"}, {"leftLeg","leftThigh"}, {"leftThigh","torso"},
                {"rightToes","rightFoot"}, {"rightFoot","rightLeg"}, {"rightLeg","rightThigh"}, {"rightThigh","torso"},
            };
            checkDepthFirstAcyclic(edgesA, parents, tree, root);
        }
    }

    SECTION("Unconnected graph.") {
        initGraph(graph, verticesB, edgesB);

        SECTION("Acyclic connected component") {
            const std::string root("B4");
            MinimumSpanningTree<std::string,std::string> tree(graph, root);

            SECTION("Check acyclic") {
                REQUIRE(tree.isUnique());
            }

            SECTION("Check NOT connected") {
                REQUIRE_FALSE(tree.isGraphConnected());
            }

            SECTION("Check Depth-first order") {
                const std::unordered_map<std::string,std::string> parents = {
                    {"B4","B4"}, {"B1","B4"}, {"B2","B1"}, {"B3","B1"},
                };
                checkDepthFirstAcyclic(edgesB, parents, tree, root);
            }
        }

        SECTION("Component with cycles") {
            const std::string root("A2");
            MinimumSpanningTree<std::string,std::string> tree(graph, root);

            SECTION("Check cycle detection") {
                REQUIRE_FALSE(tree.isUnique());
            }

            SECTION("Check NOT connected") {
                REQUIRE_FALSE(tree.isGraphConnected());
            }

            SECTION("Check Depth-first order") {
                std::vector<std::reference_wrapper<const std::string>> result;
                tree.depthFirstForEach([&result](const auto& pair) {
                    result.push_back(*pair.first);
                });
                REQUIRE(result[0].get() == root);
                REQUIRE(getIndex(result, "A3") < getIndex(result, "A4"));
            }
        }
    }
}
