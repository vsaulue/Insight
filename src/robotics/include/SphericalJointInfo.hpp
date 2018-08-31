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

#ifndef SPHERICALJOINTINFO_HPP
#define SPHERICALJOINTINFO_HPP

#include <memory>
#include <vector>

#include "btBulletDynamicsCommon.h"

#include "CompoundShape.hpp"
#include "JointInfo.hpp"
#include "units/Scalar.hpp"
#include "units/Vector3.hpp"

/**
 * Parameters to describe & construct a spherical joint.
 */
struct SphericalJointInfo : public JointInfo {
public:
    /** No initialisation constructor. */
    SphericalJointInfo() = default;

    /**
     * Full initialisation constructor.
     * @param density Density of the generated body parts.
     * @param ballTransform Relative transform of the ball in its body part.
     * @param generateBall True if the ball shape should be added to the shape of its body.
     * @param socketTransform Relative transform of the socket in its body part.
     * @param radius Radius of the ball.
     * @param startRotation Relative orientation of the ball body part to the socket part.
     * @param limits Maximum movement amplitude around each axis.
     * @param maxMotorTorque Maximum torque of the motor around each axis.
     * @param frictionCoefficients Friction factors (Torque unit / angular speed unit) around each axis.
     */
    SphericalJointInfo(Scalar<SI::Density> density, const Transform<SI::Length>& ballTransform, bool generateBall,
                       const Transform<SI::Length>& socketTransform, Scalar<SI::Length> radius,
                       const btQuaternion& startRotation, const Vector3<SI::Angle>& limits,
                       const Vector3<SI::Torque>& maxMotorTorque,
                       const Vector3<SI::AngularFrictionCoefficient>& frictionCoefficients) :
        JointInfo(density, ballTransform, generateBall, socketTransform),
        ballRadius(radius),
        startRotation(startRotation),
        limits(limits),
        maxMotorTorque(maxMotorTorque),
        frictionCoefficients(frictionCoefficients)
    {

    }

    virtual ~SphericalJointInfo() = default;

    /** Radius of the ball (m).*/
    Scalar<SI::Length> ballRadius;
    /** Relative orientation of the ball body part to the socket part. */
    btQuaternion startRotation;
    /** Maximum movement amplitude around each axis. */
    Vector3<SI::Angle> limits;
    /** Maximum torque of the motor around each axis. */
    Vector3<SI::Torque> maxMotorTorque;
    /** Friction coefficient on each axis (socket part frame). */
    Vector3<SI::AngularFrictionCoefficient> frictionCoefficients;

    void addConvexShape(std::vector<CompoundShape::ChildInfo>& shapeInfo) const override;

    std::unique_ptr<Joint> makeJoint(Body& convexPart, Body& concavePart, bool placeConvex) const override;

    /**
     * Creates a SphericalJointInfo object from the content of a Lua table.
     * @param table Lua table from which the object will be constructed.
     * @return The new SphericalJointInfo object.
     */
    static std::unique_ptr<SphericalJointInfo> luaGetFromTable(LuaTable& table);
};

#endif /* SPHERICALJOINTINFO_HPP */
