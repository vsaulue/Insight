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

/** Average density of body parts (kg/m^3).*/
static const btScalar DENSITY = 1500.0f;
/** Average density of body parts (kg/m^3). */
static const Shape::Density SHAPE_DENSITY = DENSITY;
/** Head radius (m). */
static const btScalar HEAD_RADIUS = 0.5f;
/** Arm joint ball radius (m). */
static const btScalar ARM_JOINT_BALL_RADIUS = 0.3f;
/** Radius of the cylindric part of the elbow (m). */
static const btScalar ELBOW_JOINT_CYLINDER_RADIUS = 0.12f;
/** Hip joint ball radius (m). */
static const btScalar HIP_JOINT_BALL_RADIUS = 0.4f;
/** Neck joint ball radius (m). */
static const btScalar HEAD_JOINT_BALL_RADIUS = 1.0f;
/** Radius of the cylindric part of the knee (m). */
static const btScalar KNEE_JOINT_CYLINDER_RADIUS = 0.20f;
/** Ankle ball radius (m). */
static const btScalar ANKLE_JOINT_BALL_RADIUS = 0.2f;
/** Wrist joint ball radius (m).*/
static const btScalar WRIST_JOINT_BALL_RADIUS = 0.125f;
/** Dimensions of the cylindric part of the arm. */
static const btVector3 ARM_HALF_EXTENTS = {0.1f, 0.5f, 0.1f};
/** Dimensions of the longest cylindric part of the forearm. */
static const btVector3 FOREARM_HALF_EXTENTS = {0.1, 0.55f, 0.1f};
/** Dimensions of the cylindric part of the thigh. */
static const btVector3 THIGH_HALF_EXTENTS = {0.15f, 0.6f, 0.15f};
/** Dimensions of the cylindric part of the leg. */
static const btVector3 LEG_HALF_EXTENTS = {0.15f, 0.85f, 0.15f};
/** Dimensions of the cuboid of the foot. */
static const btVector3 FOOT_HALF_EXTENTS = {0.225f, 0.1f, 0.55f};
/** Dimension of the cuboid of the toes. */
static const btVector3 TOES_HALF_EXTENTS = {0.225f, 0.1f, 0.15f};
/** Dimensions of the cuboid of the hand. */
static const btVector3 HAND_HALF_EXTENTS = {0.05f, 0.18f, 0.15f};

static const std::shared_ptr<const JointInfo> NECK = std::make_shared<const SphericalJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion(1,0,0,0), btVector3(0, 1, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion(1,0,0,0), btVector3(0, -HEAD_JOINT_BALL_RADIUS-HEAD_RADIUS, 0)), // socket transform
    HEAD_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity() // start rotation
);

static const std::shared_ptr<const JointInfo> LEFT_SHOULDER = std::make_shared<const SphericalJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion::getIdentity(), btVector3(-1.08 , 0.7, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, ARM_HALF_EXTENTS.y()+ARM_JOINT_BALL_RADIUS, 0)), // socket transform
    ARM_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity() // start rotation
);

static const std::shared_ptr<const JointInfo> RIGHT_SHOULDER = std::make_shared<const SphericalJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion::getIdentity(), btVector3(1.08 , 0.7, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, ARM_HALF_EXTENTS.y()+ARM_JOINT_BALL_RADIUS, 0)), // socket transform
    ARM_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity() // start rotation
);

static const std::shared_ptr<const JointInfo> LEFT_ELBOW = std::make_shared<const CylindricJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion(btVector3(0,1,0), SIMD_HALF_PI), btVector3(0, -ARM_HALF_EXTENTS.y(), 0)), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion(btVector3(0,1,0), SIMD_HALF_PI), btVector3(0, ARM_HALF_EXTENTS.y()+ELBOW_JOINT_CYLINDER_RADIUS, 0)), // socket transform
    ELBOW_JOINT_CYLINDER_RADIUS, // cylinder radius
    ARM_HALF_EXTENTS.x()*2, // cylinder length
    0 // start rotation
);

static const std::shared_ptr<const JointInfo> RIGHT_ELBOW = std::make_shared<const CylindricJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion(btVector3(0,1,0), -SIMD_HALF_PI), btVector3(0, -ARM_HALF_EXTENTS.y(), 0)), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion(btVector3(0,1,0), -SIMD_HALF_PI), btVector3(0, ARM_HALF_EXTENTS.y()+ELBOW_JOINT_CYLINDER_RADIUS, 0)), // socket transform
    ELBOW_JOINT_CYLINDER_RADIUS, // cylinder radius
    ARM_HALF_EXTENTS.x()*2, // cylinder length
    0 // start rotation
);

static const std::shared_ptr<const JointInfo> WRIST = std::make_shared<const SphericalJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion({0,0,1}, SIMD_PI), btVector3(0 , HAND_HALF_EXTENTS.y(), 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion({0,0,1}, SIMD_PI), btVector3(0, -ARM_HALF_EXTENTS.y()-WRIST_JOINT_BALL_RADIUS, 0)), // socket transform
    WRIST_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity() // start rotation
);

static const std::shared_ptr<const JointInfo> LEFT_HIP = std::make_shared<const SphericalJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion::getIdentity(), btVector3(-0.6, -1, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, THIGH_HALF_EXTENTS.y()+HIP_JOINT_BALL_RADIUS, 0)), // socket transform
    HIP_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity() // start rotation
);

static const std::shared_ptr<const JointInfo> RIGHT_HIP = std::make_shared<const SphericalJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion::getIdentity(), btVector3(0.6, -1, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, THIGH_HALF_EXTENTS.y()+HIP_JOINT_BALL_RADIUS, 0)), // socket transform
    HIP_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity() // start rotation
);

static const std::shared_ptr<const JointInfo> KNEE = std::make_shared<const CylindricJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion::getIdentity(), btVector3(0, -THIGH_HALF_EXTENTS.y(), 0)), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, LEG_HALF_EXTENTS.y()+KNEE_JOINT_CYLINDER_RADIUS, 0)), // socket transform
    KNEE_JOINT_CYLINDER_RADIUS, // cylinder radius
    THIGH_HALF_EXTENTS.x()*2, // cylinder length
    0 // start rotation
);

static const std::shared_ptr<const JointInfo> ANKLE = std::make_shared<const SphericalJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion({0,0,1}, SIMD_PI), btVector3(0, 0.125f, -0.2f)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion({0,0,1}, SIMD_PI), btVector3(0, -ANKLE_JOINT_BALL_RADIUS-LEG_HALF_EXTENTS.y(), 0)), // socket transform
    ANKLE_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity() // start rotation
);

static const std::shared_ptr<const JointInfo> TOES_JOINT = std::make_shared<const CylindricJointInfo>(
    DENSITY, // joint density
    btTransform(btQuaternion(btVector3(1,0,0), -SIMD_HALF_PI), btVector3(0, 0, FOOT_HALF_EXTENTS.z())), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion(btVector3(1,0,0), -SIMD_HALF_PI), btVector3(0, 0, -TOES_HALF_EXTENTS.z()-FOOT_HALF_EXTENTS.y())), // socket transform
    FOOT_HALF_EXTENTS.y(), // cylinder radius
    FOOT_HALF_EXTENTS.x()*2-0.01f, // cylinder length
    0 // start rotation
);

static const btVector3 TORSO_HALF_EXTENTS = {1,1,1};

static const std::unordered_map<std::string, std::shared_ptr<Shape>> SHAPES = {
    {"Head", std::make_shared<SphereShape>(SHAPE_DENSITY, HEAD_RADIUS)},
    {"Chest", std::make_shared<CylinderShape>(SHAPE_DENSITY, TORSO_HALF_EXTENTS)},
    {"Arm", std::make_shared<CylinderShape>(SHAPE_DENSITY, ARM_HALF_EXTENTS)},
    {"Forearm", std::make_shared<CylinderShape>(SHAPE_DENSITY, FOREARM_HALF_EXTENTS)},
    {"Thigh", std::make_shared<CylinderShape>(SHAPE_DENSITY, THIGH_HALF_EXTENTS)},
    {"Leg", std::make_shared<CylinderShape>(SHAPE_DENSITY, LEG_HALF_EXTENTS)},
    {"Foot", std::make_shared<CuboidShape>(SHAPE_DENSITY, FOOT_HALF_EXTENTS)},
    {"Toes", std::make_shared<CuboidShape>(SHAPE_DENSITY, TOES_HALF_EXTENTS)},
    {"Hand", std::make_shared<CuboidShape>(SHAPE_DENSITY, HAND_HALF_EXTENTS)},
};

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



const RobotBody::ConstructionInfo RobotBody::DEFAULT_INFO{
    {
        {"Chest",SHAPES.at("Chest")},
        {"Head", SHAPES.at("Head")},
        {"LeftArm", SHAPES.at("Arm")},
        {"RightArm", SHAPES.at("Arm")},
        {"LeftForearm", SHAPES.at("Forearm")},
        {"RightForearm", SHAPES.at("Forearm")},
        {"LeftHand", SHAPES.at("Hand")},
        {"RightHand", SHAPES.at("Hand")},
        {"LeftThigh", SHAPES.at("Thigh")},
        {"RightThigh", SHAPES.at("Thigh")},
        {"LeftLeg", SHAPES.at("Leg")},
        {"RightLeg", SHAPES.at("Leg")},
        {"LeftFoot", SHAPES.at("Foot")},
        {"RightFoot", SHAPES.at("Foot")},
        {"LeftToes", SHAPES.at("Toes")},
        {"RightToes", SHAPES.at("Toes")},
    },
    "Chest",
    {
        {"Neck", {NECK, "Chest", "Head"}},
        {"LeftShoulder", {LEFT_SHOULDER, "Chest", "LeftArm"}},
        {"RightShoulder", {RIGHT_SHOULDER, "Chest", "RightArm"}},
        {"LeftElbow", {LEFT_ELBOW, "LeftArm", "LeftForearm"}},
        {"RightElbow", {RIGHT_ELBOW, "RightArm", "RightForearm"}},
        {"LeftWrist", {WRIST, "LeftHand", "LeftForearm"}},
        {"RightWrist", {WRIST, "RightHand", "RightForearm"}},
        {"LeftHip", {LEFT_HIP, "Chest", "LeftThigh"}},
        {"RightHip", {RIGHT_HIP, "Chest", "RightThigh"}},
        {"LeftLeg", {KNEE, "LeftThigh", "LeftLeg"}},
        {"RightLeg", {KNEE, "RightThigh", "RightLeg"}},
        {"LeftFoot", {ANKLE, "LeftFoot", "LeftLeg"}},
        {"RightFoot", {ANKLE, "RightFoot", "RightLeg"}},
        {"LeftToes", {TOES_JOINT, "LeftFoot", "LeftToes"}},
        {"RightToes", {TOES_JOINT, "RightFoot", "RightToes"}},
    }
};

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
