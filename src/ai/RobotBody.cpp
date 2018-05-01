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

#include "RobotBody.hpp"
#include "SphericalJointInfo.hpp"

/** Average density of body parts (kg/m^3).*/
static const btScalar DENSITY = 1500.0f;
/** Head radius (m). */
static const btScalar HEAD_RADIUS = 0.5f;
/** Arm joint ball radius (m). */
static const btScalar ARM_JOINT_BALL_RADIUS = 0.3f;
/** Hip joint ball radius (m). */
static const btScalar HIP_JOINT_BALL_RADIUS = 0.4f;
/** Neck joint ball radius (m). */
static const btScalar HEAD_JOINT_BALL_RADIUS = 1.0f;

static const SphericalJointInfo NECK = {
    HEAD_JOINT_BALL_RADIUS, // ball radius
    btQuaternion::getIdentity(), // start rotation
    DENSITY, // joint density
    false, // place ball
    btTransform(btQuaternion(1,0,0,0), btVector3(0, 1, 0)), // ball transform
    true, // generate ball shape
    btTransform(btQuaternion(1,0,0,0), btVector3(0, -HEAD_JOINT_BALL_RADIUS-HEAD_RADIUS, 0)), // socket transform
};

static const btVector3 TORSO_HALF_EXTENTS = {1,1,1};

static std::unique_ptr<CompoundBody> createHead() {
    std::unique_ptr<CompoundBody> head = std::make_unique<CompoundBody>();
    head->addSphereD(DENSITY, HEAD_RADIUS, btVector3(0,0,0));
    return head;
}

static std::unique_ptr<CompoundBody> createChest() {
    std::unique_ptr<CompoundBody> chest = std::make_unique<CompoundBody>();
    chest->addSphereD(DENSITY, ARM_JOINT_BALL_RADIUS, btVector3(1.05 , 0.7, 0));
    chest->addSphereD(DENSITY, ARM_JOINT_BALL_RADIUS, btVector3(-1.05, 0.7, 0));
    chest->addSphereD(DENSITY, HIP_JOINT_BALL_RADIUS, btVector3(0.6, -1, 0));
    chest->addSphereD(DENSITY, HIP_JOINT_BALL_RADIUS, btVector3(-0.6, -1, 0));
    chest->addCylinderD(DENSITY, btTransform::getIdentity(), TORSO_HALF_EXTENTS);
    return chest;
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
    joints["Neck"] = std::make_unique<SphericalJoint>(chest, head, NECK);

    for (auto& part : bodyParts) {
        world.addObject(std::move(part));
    }
    for (auto& pair : joints) {
        world.addConstraint(pair.second->getConstraint());
    }
}
