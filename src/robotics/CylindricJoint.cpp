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
static void initPosition(const Body& fixedBody, const Transform<SI::Length>& fixedJoint, Body& movingBody,
                         const Transform<SI::Length>& movingJoint, Scalar<SI::Angle> initialAngle)
{
    btTransform joint;
    joint.setIdentity();
    joint.getBasis().setEulerZYX(0, 0, initialAngle.value);
    btTransform newTransform = fixedBody.getEngineTransform() * toBulletUnits(fixedJoint) * joint * toBulletUnits(movingJoint).inverse();
    movingBody.setEngineTransform(newTransform);
}

/**
 * Creates a new Bullet constraint for a cylindric joint.
 *
 * @param cylinder First body part of the joint.
 * @param socket Second body part of the joint.
 * @param info Configuration of this joint.
 * @return A Bullet constraint representing the joint.
 */
static btHingeConstraint makeConstraint(Body& cylinder, Body& socket, const CylindricJointInfo& info) {
    btRigidBody& bodyA = cylinder.getBulletBody();
    btRigidBody& bodyB = socket.getBulletBody();
    btVector3 pivotA = toBulletUnits(info.convexTransform.getOrigin());
    btVector3 pivotB = toBulletUnits(info.concaveTransform.getOrigin());
    const btVector3 axisA = info.convexTransform.getBasis() * btVector3(1,0,0);
    const btVector3 axisB = info.concaveTransform.getBasis() * btVector3(1,0,0);
    btHingeConstraint result(bodyA, bodyB, pivotA, pivotB, axisA, axisB);
    result.setLimit(toBulletUnits(info.minAngle), toBulletUnits(info.maxAngle));
    return result;
}

CylindricJoint::CylindricJoint(Body& cylinder, Body& socket, const CylindricJointInfo& info, bool placeCylinder) :
    Joint(cylinder, socket),
    jointInfo(info),
    constraint(makeConstraint(cylinder, socket, info)),
    rotationSense([this]() -> float { return this->getRotation().value; })
{
    if (placeCylinder) {
        initPosition(socket, info.concaveTransform, cylinder, info.convexTransform, info.startRotation);
    } else {
        initPosition(cylinder, info.convexTransform, socket, info.concaveTransform, -info.startRotation);
    }
}

CylindricJoint::~CylindricJoint() = default;

Scalar<SI::Angle> CylindricJoint::getRotation() {
    return Scalar<SI::Angle>(constraint.getHingeAngle());
}

SenseSignal& CylindricJoint::getRotationSense() {
    return rotationSense;
}
