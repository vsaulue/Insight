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

#include <catch.hpp>

#include "UndirectedGraph.hpp"

TEST_CASE("UndirectedGraph<std::string&,std::string>") {
    UndirectedGraph<std::string,std::string> graph;

    SECTION("newVertex") {
        const int VERTICES_COUNT = 8;
        const std::array<std::string,VERTICES_COUNT> vertices = {{
            "head","torso","leftArm","leftForearm","leftHand","rightThigh","leftThigh","rightArm",
        }};
        for (auto& vertex : vertices) {
            graph.newVertex(vertex);
        }

        SECTION("countVertices") {
            REQUIRE(graph.countVertices() == VERTICES_COUNT);
        }

        SECTION("newVertex (duplicate, must throw)") {
            REQUIRE_THROWS(graph.newVertex(vertices[5]));
        }

        SECTION("Check neighbours (0 edge case).") {
            int totalEdge2 = 0;
            for (auto& key : vertices) {
                auto vertex = graph.getVertex(key);
                for (const auto& neighbourInfo : vertex.getNeighbours()) {
                    totalEdge2++;
                }
                REQUIRE(vertex.getNeighbours().size() == 0);
            }
            REQUIRE(totalEdge2 == 0);
        }

        SECTION("addEdge") {
            //                          edge        v1          v2
            const std::array<std::tuple<std::string,std::string,std::string>,8> edges = {{
                {"neck","head","torso"},
                {"leftShoulder","torso","leftArm"},
                {"rightShoulder","rightArm","torso"},
                {"rightHip","rightThigh","torso"},
                {"leftElbow","leftArm","leftForearm"},
                {"leftHand","leftForearm","leftHand"},
                {"leftHip","leftThigh","torso"},
                {"WOLOLO","leftForearm","rightThigh"}, // have to check cycles somehow...
            }};
            for (auto& edge : edges) {
                graph.addEdge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
            }

            SECTION("addEdge (duplicate key, must throw)") {
                REQUIRE_THROWS(graph.addEdge(std::get<0>(edges[2]),"head","leftArm"));
            }

            SECTION("addEdge (duplicate vertices, must throw)") {
                auto& edge = edges[4];
                REQUIRE_THROWS(graph.addEdge("Valid",std::get<1>(edge),std::get<2>(edge)));
            }

            SECTION("Check neighbours") {
                int totalEdge2 = 0;
                for (auto& key : vertices) {
                    auto vertex = graph.getVertex(key);
                    for (const auto& neighbourInfo : vertex.getNeighbours()) {
                        const std::string& edgeName = *neighbourInfo.second;
                        const std::string& neighbourName = *neighbourInfo.first;
                        auto it = std::find_if(edges.begin(), edges.end(), [&edgeName](auto& item) -> bool {
                            return std::get<0>(item) == edgeName;
                        });
                        REQUIRE(it != edges.end());

                        bool srcVertexValid = (key == std::get<1>(*it) || key == std::get<2>(*it));
                        REQUIRE(srcVertexValid);
                        bool destVertexValid = (neighbourName == std::get<1>(*it) || neighbourName == std::get<2>(*it));
                        REQUIRE(destVertexValid);
                        totalEdge2++;
                    }
                }
                REQUIRE(totalEdge2/2 == edges.size());
            }
        }
    }
}
