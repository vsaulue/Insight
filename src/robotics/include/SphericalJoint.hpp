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

#ifndef SPHERICALJOINT_HPP
#define SPHERICALJOINT_HPP

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "Action.hpp"
#include "Body.hpp"
#include "CompoundShape.hpp"
#include "Joint.hpp"
#include "Sense.hpp"
#include "SphericalJointInfo.hpp"

/**
 * 3 degrees of freedom joint in a RobotBody.
 *
 * A spherical joint links two CompoundBody objects called:
 * <ul>
 * <li>the ball part: containing the convex (sphere) part of the joint.</li>
 * <li>the socket part: containing the concave part of the joint.</li>
 * </ul>
 */
class SphericalJoint : public Joint {
public:
    /**
     * Creates a spherical joint linking two CompoundBody objects.
     * @param ball Body part containing the convex part of the joint.
     * @param socket Body part containing the concave part of the joint.
     * @param info Configuration of this joint.
     * @param placeBall True if the ball body should be moved to match the socket body. False for the opposite.
     */
    SphericalJoint(Body& ball, Body& socket, const SphericalJointInfo& info, bool placeBall);

    virtual ~SphericalJoint();

    btTypedConstraint& getConstraint() override {
        return constraint;
    }

    void beforeTick(World& world, Scalar<BulletUnits::Time> timeStep) override;

    /**
     * Gets the relative transform of the ball in the socket frame.
     * @return The relative transform between the two parts.
     */
    btQuaternion getRotation() const;

    SenseSignal& getRotationSense() override;

    ActionSignal& getMotorAction() override;
private:
    /** Joint configuration. */
    const SphericalJointInfo& jointInfo;
    /** Bullet constraint implementing this joint.*/
    btConeTwistConstraint constraint;
    /** Sense returning the relative rotation of the ball in the socket frame. */
    Sense<btQuaternion> rotationSense;
    /** Motor/muscle power: torque exerced on the ball part by the socket part.  */
    Vector3<SI::Torque> motorTorque;
    /** Action controling the torque of the motor of this joint. */
    Action<btVector3> motorAction;

    /**
     * Sets the value of the torque exerced by this joint.
     * @param value New value.
     */
    void setMotorTorque(const Vector3<SI::Torque>& value);

    /**
     * Apply an impulse on both body parts to simulate the friction.
     *
     * @param timeStep Duration of the next integration step.
     * @param convexBasis Basis of the convex part of the joint (joint space -> absolute space).
     */
    void applyFriction(const Scalar<BulletUnits::Time> timeStep, const btMatrix3x3& convexBasis);
};

#endif /* JOINT_HPP */
