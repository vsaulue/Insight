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

#include <algorithm>

#include "CylindricJoint.hpp"
#include "units/Matrix3x3.hpp"

/** Axis of the hinge, in the joint frame. */
static const btVector3 HINGE_AXIS(1,0,0);

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
    const btVector3 axisA = info.convexTransform.getBasis() * HINGE_AXIS;
    const btVector3 axisB = info.concaveTransform.getBasis() * HINGE_AXIS;
    btHingeConstraint result(bodyA, bodyB, pivotA, pivotB, axisA, axisB);
    result.setLimit(toBulletUnits(info.minAngle), toBulletUnits(info.maxAngle));
    return result;
}

CylindricJoint::CylindricJoint(Body& cylinder, Body& socket, const CylindricJointInfo& info, bool placeCylinder) :
    Joint(cylinder, socket),
    jointInfo(info),
    constraint(makeConstraint(cylinder, socket, info)),
    rotationSense([this]() -> float { return this->getRotation().value; }),
    motorTorque(0),
    motorAction([this](const float& value) { this->setMotorTorque(Scalar<SI::Torque>(value)); })
{
    if (placeCylinder) {
        initPosition(socket, info.concaveTransform, cylinder, info.convexTransform, info.startRotation);
    } else {
        initPosition(cylinder, info.convexTransform, socket, info.concaveTransform, -info.startRotation);
    }
}

CylindricJoint::~CylindricJoint() = default;

void CylindricJoint::applyFriction(const Scalar<BulletUnits::Time> timeStep, const btMatrix3x3& convexBasis) {
    Vector3<SI::AngularVelocity> relativeVelocity = concavePart.getAngularVelocity() - convexPart.getAngularVelocity();
    Scalar<SI::AngularVelocity> refVelocity = (convexBasis.inverse() * relativeVelocity).dot(HINGE_AXIS);
    Scalar<SI::Torque> refFriction = refVelocity * jointInfo.frictionCoefficient;
    auto computeImpulse = [this,timeStep](const Body& body, const Scalar<SI::Torque>& torque) -> Scalar<SI::AngularVelocity> {
        const auto& basis = this->jointInfo.convexTransform.getBasis();
        Vector3<SI::Torque> vTorque = torque * HINGE_AXIS;
        auto vecImpulse = Matrix3x3<SI::NoUnit>(basis.transpose()).scaled(body.getInvInertiaDiagLocal()) * (basis * vTorque * timeStep * Scalar<SI::Angle>(1));
        return vecImpulse.dot(HINGE_AXIS);
    };
    Scalar<SI::AngularVelocity> convexImpulse = computeImpulse(convexPart, refFriction);
    Scalar<SI::AngularVelocity> concaveImpulse = computeImpulse(concavePart, -refFriction);
    Scalar<SI::AngularVelocity> newRefVelocity = refVelocity + concaveImpulse - convexImpulse;
    if (newRefVelocity.value * refVelocity.value < 0) {
        float factor = fabs(refVelocity.value / (refVelocity.value - newRefVelocity.value));
        convexImpulse *= factor;
        concaveImpulse *= factor;
    }
    convexPart.applyAngularImpulse(convexBasis * (convexImpulse * HINGE_AXIS));
    concavePart.applyAngularImpulse(convexBasis * (concaveImpulse * HINGE_AXIS));
}

void CylindricJoint::beforeTick(World& world, Scalar<BulletUnits::Time> timeStep) {
    btMatrix3x3 basis = convexPart.getEngineTransform().getBasis() * jointInfo.convexTransform.getBasis();
    // friction
    applyFriction(timeStep, basis);
    // motor
    btVector3 axis = basis * HINGE_AXIS;
    Vector3<SI::Torque> totalTorque = motorTorque * axis;
    convexPart.getBulletBody().applyTorque(toBulletUnits(totalTorque));
    concavePart.getBulletBody().applyTorque(-toBulletUnits(totalTorque));
}

Scalar<SI::Angle> CylindricJoint::getRotation() {
    return Scalar<SI::Angle>(constraint.getHingeAngle());
}

SenseSignal& CylindricJoint::getRotationSense() {
    return rotationSense;
}

ActionSignal& CylindricJoint::getMotorAction() {
    return motorAction;
}

void CylindricJoint::setMotorTorque(Scalar<SI::Torque> value) {
    motorTorque = std::clamp(value, -jointInfo.maxMotorTorque, jointInfo.maxMotorTorque);
    concavePart.getBulletBody().activate();
    convexPart.getBulletBody().activate();
}
