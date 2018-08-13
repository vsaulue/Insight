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

#include "SphericalJointFeedbackLoop.hpp"

SphericalJointFeedbackLoop::SphericalJointFeedbackLoop(const Sense<btQuaternion>& sense, Action<btVector3>& action) :
    inputRotation(sense),
    outputMotorTorque(action),
    targetRotation(btQuaternion::getIdentity()),
    previousRotation(inputRotation.get())
{

}

SphericalJointFeedbackLoop::~SphericalJointFeedbackLoop() = default;

/**
 * Gets the angle of a rotation in ]-PI:PI] range.
 * @param quaternion Input rotation.
 * @return The angle (between -PI and PI) of the rotation represented by the given quaternion
 */
static btScalar getAngle(const btQuaternion& quaternion) {
    btScalar result = quaternion.getAngle();
    if (result > SIMD_PI) {
        result -= SIMD_2_PI;
    }
    return result;
}

void SphericalJointFeedbackLoop::stepSimulation() {
    btQuaternion newRotation = inputRotation.get();
    btQuaternion delta = newRotation * targetRotation.inverse();
    btVector3 torque = -0.2 * getAngle(delta) * delta.getAxis();
    btQuaternion speed = newRotation * previousRotation.inverse();
    torque-= getAngle(speed) * speed.getAxis();

    outputMotorTorque.set(torque);
    previousRotation = newRotation;
}

int SphericalJointFeedbackLoop::luaIndex(const std::string& memberName, LuaStateView& state) {
    return 0;
}
