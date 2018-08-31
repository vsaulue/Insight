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

#include "SphericalJoint.hpp"
#include "units/Matrix3x3.hpp"

static void initPosition(const Body& fixedBody, const Transform<SI::Length>& fixedJoint, Body& movingBody,
                         const Transform<SI::Length>& movingJoint, const btQuaternion& rotation)
{
    btTransform joint(rotation, btVector3(0,0,0));
    btTransform newTransform = fixedBody.getEngineTransform() * toBulletUnits(fixedJoint) * joint * toBulletUnits(movingJoint).inverse();
    movingBody.setEngineTransform(newTransform);
}

static btConeTwistConstraint makeConstraint(Body& ball, Body& socket, const SphericalJointInfo& info) {
    btTransform convexTr = toBulletUnits(info.convexTransform);
    btTransform concaveTr = toBulletUnits(info.concaveTransform);
    btConeTwistConstraint result(ball.getBulletBody(), socket.getBulletBody(), convexTr, concaveTr);
    btVector3 limits = toBulletUnits(info.limits);
    result.setLimit(limits.z(), limits.y(), limits.x(), 1, 0.01);
    return result;
}

SphericalJoint::SphericalJoint(Body& ball, Body& socket, const SphericalJointInfo& info, bool placeBall) :
    Joint(ball, socket),
    jointInfo(info),
    constraint(makeConstraint(ball, socket, info)),
    rotationSense([this]() -> btQuaternion { return this->getRotation(); }),
    motorTorque(0,0,0),
    motorAction([this](const btVector3& value) { this->setMotorTorque(Vector3<SI::Torque>(value)); })
{
    if (placeBall) {
        initPosition(socket, info.concaveTransform, ball, info.convexTransform, info.startRotation);
    } else {
        initPosition(ball, info.convexTransform, socket, info.concaveTransform, info.startRotation.inverse());
    }
}

SphericalJoint::~SphericalJoint() = default;

void SphericalJoint::applyFriction(const Scalar<BulletUnits::Time> timeStep, const btMatrix3x3& convexBasis) {
    Vector3<SI::AngularVelocity> relativeVelocity = concavePart.getAngularVelocity() - convexPart.getAngularVelocity();
    Vector3<SI::AngularVelocity> refVelocity = convexBasis.inverse() * relativeVelocity;
    Vector3<SI::Torque> refFriction(
        refVelocity.x() * jointInfo.frictionCoefficients.x(),
        refVelocity.y() * jointInfo.frictionCoefficients.y(),
        refVelocity.z() * jointInfo.frictionCoefficients.z()
    );
    auto computeImpulse = [this,timeStep](const Body& body, const Vector3<SI::Torque>& torque) -> Vector3<SI::AngularVelocity> {
        const auto& basis = this->jointInfo.convexTransform.getBasis();
        return Matrix3x3<SI::NoUnit>(basis.transpose()).scaled(body.getInvInertiaDiagLocal()) * (basis * torque * timeStep * Scalar<SI::Angle>(1));
    };
    Vector3<SI::AngularVelocity> convexImpulse = computeImpulse(convexPart, refFriction);
    Vector3<SI::AngularVelocity> concaveImpulse = computeImpulse(concavePart, -refFriction);
    Vector3<SI::AngularVelocity> newRelVelocity = refVelocity + concaveImpulse - convexImpulse;
    for (int i=0; i<3; i++) {
        auto prevVel = refVelocity.value.m_floats[i];
        auto newVel = newRelVelocity.value.m_floats[i];
        if (newVel * prevVel < 0) {
            float factor = fabs(prevVel / (prevVel - newVel));
            convexImpulse.value.m_floats[i] *= factor;
            concaveImpulse.value.m_floats[i] *= factor;
        }
    }
    convexPart.applyAngularImpulse(convexBasis * convexImpulse);
    concavePart.applyAngularImpulse(convexBasis * concaveImpulse);
}

void SphericalJoint::beforeTick(World& world, Scalar<BulletUnits::Time> timeStep) {
    btMatrix3x3 basis = convexPart.getEngineTransform().getBasis() * jointInfo.convexTransform.getBasis();
    // Friction
    applyFriction(timeStep, basis);
    // Motor
    Vector3<SI::Torque> absMotorTorque = basis * motorTorque;
    convexPart.getBulletBody().applyTorque(toBulletUnits(absMotorTorque));
    concavePart.getBulletBody().applyTorque(-toBulletUnits(absMotorTorque));
}

btQuaternion SphericalJoint::getRotation() const {
    auto absBall = constraint.getRigidBodyA().getWorldTransform().getRotation() * constraint.getFrameOffsetA().getRotation();
    auto absSocket = constraint.getRigidBodyB().getWorldTransform().getRotation() * constraint.getFrameOffsetB().getRotation();
    return absSocket.inverse() * absBall;
}

SenseSignal& SphericalJoint::getRotationSense() {
    return rotationSense;
}

ActionSignal& SphericalJoint::getMotorAction() {
    return motorAction;
}

void SphericalJoint::setMotorTorque(const Vector3<SI::Torque>& value) {
    const auto& maxTorque = jointInfo.maxMotorTorque;
    motorTorque = {
        std::clamp(value.x(), -maxTorque.x(), maxTorque.x()),
        std::clamp(value.y(), -maxTorque.y(), maxTorque.y()),
        std::clamp(value.z(), -maxTorque.z(), maxTorque.z()),
    };
    convexPart.getBulletBody().activate();
    concavePart.getBulletBody().activate();
}
