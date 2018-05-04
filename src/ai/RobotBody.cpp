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

#include <vector>

#include "CylindricJoint.hpp"
#include "CylindricJointInfo.hpp"
#include "RobotBody.hpp"
#include "SphericalJointInfo.hpp"

/** Average density of body parts (kg/m^3).*/
static const btScalar DENSITY = 1500.0f;
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
/** Dimensions of the cylindric part of the arm. */
static const btVector3 ARM_HALF_EXTENTS = {0.1f, 0.5f, 0.1f};
/** Dimensions of the longest cylindric part of the forearm. */
static const btVector3 FOREARM_HALF_EXTENTS = {0.1, 0.55f, 0.1f};
/** Dimensions of the cylindric part of the thigh. */
static const btVector3 THIGH_HALF_EXTENTS = {0.15f, 0.6f, 0.15f};
/** Dimensions of the cylindric part of the leg. */
static const btVector3 LEG_HALF_EXTENTS = {0.15f, 0.85f, 0.15f};

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
    btTransform(btQuaternion::getIdentity(), btVector3(0, ARM_HALF_EXTENTS.y()+ELBOW_JOINT_CYLINDER_RADIUS, 0)), // socket transform
};

static const CylindricJointInfo RIGHT_ELBOW = {
    ELBOW_JOINT_CYLINDER_RADIUS, // cylinder radius
    ARM_HALF_EXTENTS.x()*2, // cylinder length
    0, // start rotation
    DENSITY, // joint density
    false, // place cylinder
    btTransform(btQuaternion(btVector3(0,1,0), -SIMD_HALF_PI), btVector3(0, -ARM_HALF_EXTENTS.y(), 0)), // cylinder transform
    true, // generate cylinder shape
    btTransform(btQuaternion::getIdentity(), btVector3(0, ARM_HALF_EXTENTS.y()+ELBOW_JOINT_CYLINDER_RADIUS, 0)), // socket transform
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

static const btVector3 TORSO_HALF_EXTENTS = {1,1,1};

static std::unique_ptr<CompoundBody> createHead() {
    std::unique_ptr<CompoundBody> head = std::make_unique<CompoundBody>();
    head->addSphereD(DENSITY, HEAD_RADIUS, btVector3(0,0,0));
    return head;
}

static std::unique_ptr<CompoundBody> createChest() {
    std::unique_ptr<CompoundBody> chest = std::make_unique<CompoundBody>();
    chest->addCylinderD(DENSITY, btTransform::getIdentity(), TORSO_HALF_EXTENTS);
    return chest;
}

static std::unique_ptr<CompoundBody> createArm() {
    std::unique_ptr<CompoundBody> arm = std::make_unique<CompoundBody>();
    arm->addCylinderD(DENSITY, btTransform::getIdentity(), ARM_HALF_EXTENTS);
    return arm;
}

static std::unique_ptr<CompoundBody> createForearm() {
    std::unique_ptr<CompoundBody> forearm = std::make_unique<CompoundBody>();
    forearm->addCylinderD(DENSITY, btTransform::getIdentity(), FOREARM_HALF_EXTENTS);
    return forearm;
}

static std::unique_ptr<CompoundBody> createThigh() {
    std::unique_ptr<CompoundBody> thigh = std::make_unique<CompoundBody>();
    thigh->addCylinderD(DENSITY, btTransform::getIdentity(), THIGH_HALF_EXTENTS);
    return thigh;
}

static std::unique_ptr<CompoundBody> createLeg() {
    std::unique_ptr<CompoundBody> leg = std::make_unique<CompoundBody>();
    leg->addCylinderD(DENSITY, btTransform::getIdentity(), LEG_HALF_EXTENTS);
    return leg;
}

RobotBody::RobotBody(World& world) {
    std::vector<std::unique_ptr<CompoundBody>> bodyParts;
    auto newPart = [this, &bodyParts](const std::string& name, std::unique_ptr<CompoundBody> part) -> CompoundBody& {
        CompoundBody& result = *part;
        bodyParts.push_back(std::move(part));
        return result;
    };
    CompoundBody& chest = newPart("Chest", createChest());
    CompoundBody& head = newPart("Head", createHead());
    CompoundBody& leftArm = newPart("LeftArm", createArm());
    CompoundBody& rightArm = newPart("RightArm", createArm());
    CompoundBody& leftForearm = newPart("LeftForearm", createForearm());
    CompoundBody& rightForearm = newPart("RightForearm", createForearm());
    CompoundBody& leftThigh = newPart("LeftThigh", createThigh());
    CompoundBody& rightThigh = newPart("RightThigh", createThigh());
    CompoundBody& leftLeg = newPart("LeftLeg", createLeg());
    CompoundBody& rightLeg = newPart("RightLeg", createLeg());
    joints["Neck"] = std::make_unique<SphericalJoint>(chest, head, NECK);
    joints["LeftShoulder"] = std::make_unique<SphericalJoint>(chest, leftArm, LEFT_SHOULDER);
    joints["RightShoulder"] = std::make_unique<SphericalJoint>(chest, rightArm, RIGHT_SHOULDER);
    joints["LeftElbow"] = std::make_unique<CylindricJoint>(leftArm, leftForearm, LEFT_ELBOW);
    joints["RightElbow"] = std::make_unique<CylindricJoint>(rightArm, rightForearm, RIGHT_ELBOW);
    joints["LeftHip"] = std::make_unique<SphericalJoint>(chest, leftThigh, LEFT_HIP);
    joints["RightHip"] = std::make_unique<SphericalJoint>(chest, rightThigh, RIGHT_HIP);
    joints["LeftLeg"] = std::make_unique<CylindricJoint>(leftThigh, leftLeg, KNEE);
    joints["RightLeg"] = std::make_unique<CylindricJoint>(rightThigh, rightLeg, KNEE);

    for (auto& part : bodyParts) {
        world.addObject(std::move(part));
    }
    for (auto& pair : joints) {
        world.addConstraint(pair.second->getConstraint());
    }
}
