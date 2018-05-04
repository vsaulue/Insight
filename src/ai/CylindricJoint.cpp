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

#include "CylindricJoint.hpp"

/**
 * Places a body part near another to align their joint parts.
 *
 * @param[in] fixedBody Immobile body.
 * @param[in] fixedJoint Relative transform of the joint in fixedBody.
 * @param[out] movingBody Body to move in order to match the joint of fixedBody.
 * @param[in] movingJoint Relative transform of the joint in movingBody.
 * @param[in] initialAngle Initial angle of the bodies at the joint.
 */
static void initPosition(const CompoundBody& fixedBody, const btTransform& fixedJoint, CompoundBody& movingBody, const btTransform& movingJoint, btScalar initialAngle) {
    btTransform joint;
    joint.setIdentity();
    joint.getBasis().setEulerZYX(0, 0, initialAngle);
    btTransform newTransform = fixedBody.getTransform() * fixedJoint * joint * movingJoint.inverse();
    movingBody.setTransform(newTransform);
}

/**
 * Creates a new Bullet constraint for a cylindric joint.
 *
 * @param cylinder First body part of the joint.
 * @param socket Second body part of the joint.
 * @param info Configuration of this joint.
 * @return A Bullet constraint representing the joint.
 */
static std::shared_ptr<btHingeConstraint> makeConstraint(CompoundBody& cylinder, CompoundBody& socket, const CylindricJointInfo& info) {
    btRigidBody& bodyA = *cylinder.getBulletBody();
    btRigidBody& bodyB = *socket.getBulletBody();
    const btVector3& pivotA = info.cylinderTransform.getOrigin();
    const btVector3& pivotB = info.socketTransform.getOrigin();
    const btVector3 axisA = info.cylinderTransform.getBasis() * btVector3(1,0,0);
    const btVector3 axisB = info.socketTransform.getBasis() * btVector3(1,0,0);
    return std::make_shared<btHingeConstraint>(bodyA, bodyB, pivotA, pivotB, axisA, axisB);
}

CylindricJoint::CylindricJoint(CompoundBody& cylinder, CompoundBody& socket, const CylindricJointInfo& info) :
    cylinder(cylinder),
    socket(socket),
    jointInfo(info),
    constraint(makeConstraint(cylinder, socket, info))
{
    if (info.generateCylinder) {
        btVector3 halfExtents(info.cylinderRadius, info.cylinderLength/2, info.cylinderRadius);
        btTransform transform = info.cylinderTransform * btTransform(btQuaternion(btVector3(0,0,1), SIMD_HALF_PI));
        cylinder.addCylinderD(info.jointDensity, transform, halfExtents);
    }
    if (info.placeCylinder) {
        initPosition(socket, info.socketTransform, cylinder, info.cylinderTransform, info.startRotation);
    } else {
        initPosition(cylinder, info.cylinderTransform, socket, info.socketTransform, -info.startRotation);
    }
}

CylindricJoint::~CylindricJoint() = default;
