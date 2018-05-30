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

#include <memory>
#include <tuple>
#include <vector>

#include "CuboidShape.hpp"
#include "CylinderShape.hpp"
#include "CylindricJoint.hpp"
#include "CylindricJointInfo.hpp"
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/bindings/luaVirtualClass/shared_ptr.hpp"
#include "lua/bindings/std/string.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "MinimumSpanningTree.hpp"
#include "RobotBody.hpp"
#include "SphereShape.hpp"
#include "SphericalJoint.hpp"
#include "SphericalJointInfo.hpp"
#include "UndirectedGraph.hpp"

RobotBody::ConstructionInfo::ConstructionInfo(const std::unordered_map<std::string, std::shared_ptr<Shape>>& parts,
                                              const std::string& basePartName,
                                              const std::unordered_map<std::string, JointInputData>& joints) :
    basePartName(basePartName)
{
    UndirectedGraph<std::string,std::string> graph;
    std::unordered_map<std::string,std::vector<CompoundShape::ChildInfo>> shapeInfos;

    if (parts.find(basePartName) == parts.end()) {
        std::string msg = std::string("Unkown part name in basePartName: ") + basePartName;
        throw std::out_of_range(msg);
    }

    for (const auto& pair : parts) {
        graph.newVertex(pair.first);
    }

    auto checkPart = [&parts](const std::string& name) {
        const auto it = parts.find(name);
        if (it == parts.end()) {
            std::string msg = std::string("Unkown part name in joint list: ") + name;
            throw std::out_of_range(msg);
        }
    };

    for (const auto& pair : joints) {
        const JointInfo& jointInfo = *pair.second.info;
        checkPart(pair.second.convexPartName);
        checkPart(pair.second.concavePartName);
        jointInfo.addConvexShape(shapeInfos[pair.second.convexPartName]);
        graph.addEdge(pair.first, pair.second.convexPartName, pair.second.concavePartName);
    }

    MinimumSpanningTree<std::string,std::string> spanningTree(graph, basePartName);
    if (!spanningTree.isUnique()) {
        throw std::invalid_argument("Cycle of joints detected.");
    }
    if (!spanningTree.isGraphConnected()) {
        throw std::invalid_argument("All the body parts are not connected with joints.");
    }

    this->joints.reserve(joints.size());
    spanningTree.depthFirstForEach([&joints,this](const auto& pair) {
        if (pair.second != nullptr) {
            const JointInputData& joint = joints.at(*pair.second);
            JointData data = {
                *pair.second,                        // jointName
                joint.info,                          // jointInfo
                joint.convexPartName,                // convexPartName
                joint.concavePartName,               // concavePartName
                joint.convexPartName == *pair.first, // placeConvex
            };
            this->joints.push_back(data);
        }
    });

    for (const auto& pair : parts) {
        auto it = shapeInfos.find(pair.first);
        if (it == shapeInfos.end()) {
            this->parts[pair.first] = pair.second;
        } else {
            auto& childInfos = it->second;
            childInfos.push_back({pair.second, btTransform::getIdentity()});
            this->parts[pair.first] = std::make_shared<CompoundShape>(childInfos);
        }
    }
}

const std::unordered_map<std::string, std::shared_ptr<Shape>> RobotBody::ConstructionInfo::getParts() const {
    return parts;
}

const std::string& RobotBody::ConstructionInfo::getBasePartName() const {
    return basePartName;
}

const std::vector<RobotBody::ConstructionInfo::JointData>& RobotBody::ConstructionInfo::getJoints() const {
    return joints;
}

RobotBody::RobotBody(World& world, const ConstructionInfo& info) {
    for (const auto& pair : info.getParts()) {
        std::shared_ptr<Body> body = std::make_shared<Body>(pair.second);
        if (pair.first == info.getBasePartName()) {
            baseBody = body.get();
        }
        parts[pair.first] = body;
    }
    for (const auto& jointData : info.getJoints()) {
        const JointInfo& info = *jointData.jointInfo;
        Body& convexPart = *parts[jointData.convexPartName];
        Body& concavePart = *parts[jointData.concavePartName];
        joints[jointData.jointName] = info.makeJoint(convexPart, concavePart, jointData.placeConvex);
    }

    for (auto& pair : parts) {
        world.addObject(pair.second);
    }
    for (auto& pair : joints) {
        world.addConstraint(pair.second->getConstraint());
    }
}

RobotBody::~RobotBody() = default;

int RobotBody::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<RobotBody>;
    int result = 1;
    if (memberName=="position") {
        state.push<btVector3>(baseBody->getTransform().getOrigin());
    } else if (memberName=="setPosition") {
        state.push<Method>([](RobotBody& object, LuaStateView& state) -> int {
            Body& base = *object.baseBody;
            btVector3 newPos = state.get<btVector3>(2);
            btVector3 translation = newPos - base.getTransform().getOrigin();
            for (auto& part : object.parts) {
                part.second->setPosition(part.second->getTransform().getOrigin() + translation);
            }
            return 0;
        });
    } else if (memberName=="rotation") {
        state.push<btQuaternion>(baseBody->getTransform().getRotation());
    } else if (memberName=="setRotation") {
        state.push<Method>([](RobotBody& object, LuaStateView& state) -> int {
            Body& base = *object.baseBody;
            const btQuaternion curRotation = base.getTransform().getRotation();
            btTransform relTransform(state.get<btQuaternion>(2)*curRotation.inverse());
            for (auto& part : object.parts) {
                part.second->setTransform(relTransform*part.second->getTransform());
            }
            return 0;
        });
    } else {
        result = 0;
    }
    return result;
}
