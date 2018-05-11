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

#include <tuple>
#include <vector>

#include "CuboidShape.hpp"
#include "CylinderShape.hpp"
#include "CylindricJoint.hpp"
#include "CylindricJointInfo.hpp"
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/types/LuaMethod.hpp"
#include "RobotBody.hpp"
#include "SphereShape.hpp"
#include "SphericalJoint.hpp"
#include "SphericalJointInfo.hpp"

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

static const SphericalJointInfo NECK = {
    HEAD_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity(), // start rotation
    DENSITY, // joint density
    false, // place ball
    btTransform(btQuaternion(1,0,0,0), btVector3(0, 1, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion(1,0,0,0), btVector3(0, -HEAD_JOINT_BALL_RADIUS-HEAD_RADIUS, 0)), // socket transform
};

static const SphericalJointInfo LEFT_SHOULDER = {
    ARM_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity(), // start rotation
    DENSITY, // joint density
    false, // place ball
    btTransform(btQuaternion::getIdentity(), btVector3(-1.08 , 0.7, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, ARM_HALF_EXTENTS.y()+ARM_JOINT_BALL_RADIUS, 0)), // socket transform
};

static const SphericalJointInfo RIGHT_SHOULDER = {
    ARM_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity(), // start rotation
    DENSITY, // joint density
    false, // place ball
    btTransform(btQuaternion::getIdentity(), btVector3(1.08 , 0.7, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, ARM_HALF_EXTENTS.y()+ARM_JOINT_BALL_RADIUS, 0)), // socket transform
};

static const CylindricJointInfo LEFT_ELBOW = {
    ELBOW_JOINT_CYLINDER_RADIUS, // cylinder radius
    ARM_HALF_EXTENTS.x()*2, // cylinder length
    0, // start rotation
    DENSITY, // joint density
    false, // place cylinder
    btTransform(btQuaternion(btVector3(0,1,0), SIMD_HALF_PI), btVector3(0, -ARM_HALF_EXTENTS.y(), 0)), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion(btVector3(0,1,0), SIMD_HALF_PI), btVector3(0, ARM_HALF_EXTENTS.y()+ELBOW_JOINT_CYLINDER_RADIUS, 0)), // socket transform
};

static const CylindricJointInfo RIGHT_ELBOW = {
    ELBOW_JOINT_CYLINDER_RADIUS, // cylinder radius
    ARM_HALF_EXTENTS.x()*2, // cylinder length
    0, // start rotation
    DENSITY, // joint density
    false, // place cylinder
    btTransform(btQuaternion(btVector3(0,1,0), -SIMD_HALF_PI), btVector3(0, -ARM_HALF_EXTENTS.y(), 0)), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion(btVector3(0,1,0), -SIMD_HALF_PI), btVector3(0, ARM_HALF_EXTENTS.y()+ELBOW_JOINT_CYLINDER_RADIUS, 0)), // socket transform
};

static const SphericalJointInfo WRIST = {
    WRIST_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity(), // start rotation
    DENSITY, // joint density
    true, // place ball
    btTransform(btQuaternion({0,0,1}, SIMD_PI), btVector3(0 , HAND_HALF_EXTENTS.y(), 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion({0,0,1}, SIMD_PI), btVector3(0, -ARM_HALF_EXTENTS.y()-WRIST_JOINT_BALL_RADIUS, 0)), // socket transform
};

static const SphericalJointInfo LEFT_HIP = {
    HIP_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity(), // start rotation
    DENSITY, // joint density
    false, // place ball
    btTransform(btQuaternion::getIdentity(), btVector3(-0.6, -1, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, THIGH_HALF_EXTENTS.y()+HIP_JOINT_BALL_RADIUS, 0)), // socket transform
};

static const SphericalJointInfo RIGHT_HIP = {
    HIP_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity(), // start rotation
    DENSITY, // joint density
    false, // place ball
    btTransform(btQuaternion::getIdentity(), btVector3(0.6, -1, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, THIGH_HALF_EXTENTS.y()+HIP_JOINT_BALL_RADIUS, 0)), // socket transform
};

static const CylindricJointInfo KNEE = {
    KNEE_JOINT_CYLINDER_RADIUS, // cylinder radius
    THIGH_HALF_EXTENTS.x()*2, // cylinder length
    0, // start rotation
    DENSITY, // joint density
    false, // place cylinder
    btTransform(btQuaternion::getIdentity(), btVector3(0, -THIGH_HALF_EXTENTS.y(), 0)), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, LEG_HALF_EXTENTS.y()+KNEE_JOINT_CYLINDER_RADIUS, 0)), // socket transform
};

static const SphericalJointInfo ANKLE = {
    ANKLE_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity(), // start rotation
    DENSITY, // joint density
    true, // place ball
    btTransform(btQuaternion({0,0,1}, SIMD_PI), btVector3(0, 0.125f, -0.2f)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion({0,0,1}, SIMD_PI), btVector3(0, -ANKLE_JOINT_BALL_RADIUS-LEG_HALF_EXTENTS.y(), 0)), // socket transform
};

static const CylindricJointInfo TOES_JOINT = {
    FOOT_HALF_EXTENTS.y(), // cylinder radius
    FOOT_HALF_EXTENTS.x()*2-0.01f, // cylinder length
    0, // start rotation
    DENSITY, // joint density
    false, // place cylinder
    btTransform(btQuaternion(btVector3(1,0,0), -SIMD_HALF_PI), btVector3(0, 0, FOOT_HALF_EXTENTS.z())), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion(btVector3(1,0,0), -SIMD_HALF_PI), btVector3(0, 0, -TOES_HALF_EXTENTS.z()-FOOT_HALF_EXTENTS.y())), // socket transform
};

static const btVector3 TORSO_HALF_EXTENTS = {1,1,1};

static const std::unordered_map<std::string, std::tuple<const SphericalJointInfo&,std::string>> SPHERICAL_JOINTS = {
    {"Ankle", {ANKLE, "Foot"}},
    {"Wrist", {WRIST, "Hand"}},
    {"RightHip", {RIGHT_HIP, "Chest"}},
    {"LeftHip", {LEFT_HIP, "Chest"}},
    {"RightShoulder", {RIGHT_SHOULDER, "Chest"}},
    {"LeftShoulder", {LEFT_SHOULDER, "Chest"}},
    {"Neck", {NECK, "Chest"}},
};

static const std::unordered_map<std::string, std::tuple<const CylindricJointInfo&,std::string>> CYLINDRIC_JOINTS = {
    {"Toes", {TOES_JOINT, "Foot"}},
    {"Knee", {KNEE, "Thigh"}},
    {"Elbow", {RIGHT_ELBOW, "Arm"}},
};

static std::unordered_map<std::string, std::shared_ptr<CompoundShape>> initShapes() {
    std::unordered_map<std::string, std::vector<CompoundShape::ChildInfo>> shapesInfo = {
        {"Head", {{std::make_shared<SphereShape>(SHAPE_DENSITY, HEAD_RADIUS), btTransform::getIdentity() }}},
        {"Chest", {{std::make_shared<CylinderShape>(SHAPE_DENSITY, TORSO_HALF_EXTENTS), btTransform::getIdentity() }}},
        {"Arm", {{std::make_shared<CylinderShape>(SHAPE_DENSITY, ARM_HALF_EXTENTS), btTransform::getIdentity() }}},
        {"Forearm", {{std::make_shared<CylinderShape>(SHAPE_DENSITY, FOREARM_HALF_EXTENTS), btTransform::getIdentity() }}},
        {"Thigh", {{std::make_shared<CylinderShape>(SHAPE_DENSITY, THIGH_HALF_EXTENTS), btTransform::getIdentity() }}},
        {"Leg", {{std::make_shared<CylinderShape>(SHAPE_DENSITY, LEG_HALF_EXTENTS), btTransform::getIdentity() }}},
        {"Foot", {{std::make_shared<CuboidShape>(SHAPE_DENSITY, FOOT_HALF_EXTENTS), btTransform::getIdentity() }}},
        {"Toes", {{std::make_shared<CuboidShape>(SHAPE_DENSITY, TOES_HALF_EXTENTS), btTransform::getIdentity() }}},
        {"Hand", {{std::make_shared<CuboidShape>(SHAPE_DENSITY, HAND_HALF_EXTENTS), btTransform::getIdentity() }}},
    };
    for (const auto& joint : SPHERICAL_JOINTS) {
        std::vector<CompoundShape::ChildInfo>& sphereShapeInfo = shapesInfo[std::get<1>(joint.second)];
        const SphericalJointInfo& jointInfo = std::get<0>(joint.second);
        jointInfo.addSphereShape(sphereShapeInfo);
    }
    for (const auto& joint : CYLINDRIC_JOINTS) {
        std::vector<CompoundShape::ChildInfo>& cylinderShapeInfo = shapesInfo[std::get<1>(joint.second)];
        const CylindricJointInfo& jointInfo = std::get<0>(joint.second);
        jointInfo.addCylinderShape(cylinderShapeInfo);
    }
    std::unordered_map<std::string, std::shared_ptr<CompoundShape>> result;
    for (auto& shape : shapesInfo) {
        result[shape.first] = std::make_shared<CompoundShape>(shape.second);
    }
    return result;
}

static const std::unordered_map<std::string, std::shared_ptr<CompoundShape>> SHAPES = initShapes();

RobotBody::RobotBody(World& world) {
    std::vector<std::unique_ptr<Body>> bodyParts;
    auto newPart = [this, &bodyParts](const std::string& partName, std::string shapeName) -> Body& {
        std::unique_ptr<Body> part = std::make_unique<Body>(SHAPES.at(shapeName));
        Body& result = *part;
        this->parts[partName] = part.get();
        bodyParts.push_back(std::move(part));
        return result;
    };
    Body& chest = newPart("Chest", "Chest");
    Body& head = newPart("Head", "Head");
    Body& leftArm = newPart("LeftArm", "Arm");
    Body& rightArm = newPart("RightArm", "Arm");
    Body& leftForearm = newPart("LeftForearm", "Forearm");
    Body& rightForearm = newPart("RightForearm", "Forearm");
    Body& leftHand = newPart("LeftHand", "Hand");
    Body& rightHand = newPart("RightHand", "Hand");
    Body& leftThigh = newPart("LeftThigh", "Thigh");
    Body& rightThigh = newPart("RightThigh", "Thigh");
    Body& leftLeg = newPart("LeftLeg", "Leg");
    Body& rightLeg = newPart("RightLeg", "Leg");
    Body& leftFoot = newPart("LeftFoot", "Foot");
    Body& rightFoot = newPart("RightFoot", "Foot");
    Body& leftToes = newPart("LeftToes", "Toes");
    Body& rightToes = newPart("RightToes", "Toes");
    joints["Neck"] = std::make_unique<SphericalJoint>(chest, head, NECK);
    joints["LeftShoulder"] = std::make_unique<SphericalJoint>(chest, leftArm, LEFT_SHOULDER);
    joints["RightShoulder"] = std::make_unique<SphericalJoint>(chest, rightArm, RIGHT_SHOULDER);
    joints["LeftElbow"] = std::make_unique<CylindricJoint>(leftArm, leftForearm, LEFT_ELBOW);
    joints["RightElbow"] = std::make_unique<CylindricJoint>(rightArm, rightForearm, RIGHT_ELBOW);
    joints["LeftWrist"] = std::make_unique<SphericalJoint>(leftHand, leftForearm, WRIST);
    joints["RightWrist"] = std::make_unique<SphericalJoint>(rightHand, rightForearm, WRIST);
    joints["LeftHip"] = std::make_unique<SphericalJoint>(chest, leftThigh, LEFT_HIP);
    joints["RightHip"] = std::make_unique<SphericalJoint>(chest, rightThigh, RIGHT_HIP);
    joints["LeftLeg"] = std::make_unique<CylindricJoint>(leftThigh, leftLeg, KNEE);
    joints["RightLeg"] = std::make_unique<CylindricJoint>(rightThigh, rightLeg, KNEE);
    joints["LeftFoot"] = std::make_unique<SphericalJoint>(leftFoot, leftLeg, ANKLE);
    joints["RightFoot"] = std::make_unique<SphericalJoint>(rightFoot, rightLeg, ANKLE);
    joints["LeftToes"] = std::make_unique<CylindricJoint>(leftFoot, leftToes, TOES_JOINT);
    joints["RightToes"] = std::make_unique<CylindricJoint>(rightFoot, rightToes, TOES_JOINT);

    for (auto& part : bodyParts) {
        world.addObject(std::move(part));
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
        state.push<btVector3>(getBaseBody().getTransform().getOrigin());
    } else if (memberName=="setPosition") {
        state.push<Method>([](RobotBody& object, LuaStateView& state) -> int {
            Body& base = object.getBaseBody();
            btVector3 newPos = state.get<btVector3>(2);
            btVector3 translation = newPos - base.getTransform().getOrigin();
            for (auto& part : object.parts) {
                part.second->setPosition(part.second->getTransform().getOrigin() + translation);
            }
            return 0;
        });
    } else if (memberName=="rotation") {
        state.push<btQuaternion>(getBaseBody().getTransform().getRotation());
    } else if (memberName=="setRotation") {
        state.push<Method>([](RobotBody& object, LuaStateView& state) -> int {
            Body& base = object.getBaseBody();
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

Body& RobotBody::getBaseBody() {
    return *parts["Chest"];
}
