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

#ifndef CYLINDRICJOINTINFO_HPP
#define CYLINDRICJOINTINFO_HPP

#include <memory>
#include <vector>

#include "btBulletDynamicsCommon.h"

#include "CompoundShape.hpp"
#include "JointInfo.hpp"
#include "lua/types/LuaTable.hpp"

/**
 * Parameters to describe & construct a cylindric joint.
 */
struct CylindricJointInfo : public JointInfo {
    /** No intialisation constructor. */
    CylindricJointInfo() = default;

    /**
     * Full initialisation constructor.
     * @param density Density of the generated body parts.
     * @param cylinderTransform Relative transform of the ball in its body part.
     * @param generateCylinder True if the ball shape should be added to the shape of its body.
     * @param socketTransform Relative transform of the socket in its body part.
     * @param radius Radius of the cylinder.
     * @param length Length of the cylinder.
     * @param startRotation Relative orientation of the cylinder body part to the socket part (radian).
     * @param minAngle Minimum possible value for the angle between the two parts.
     * @param maxAngle Maximum possible value for the angle between the two parts.
     * @param maxMotorTorque Maximum torque that the motor of this joint between the two bodies.
     * @param frictionCoefficient Friction factor (Torque unit / angular speed unit).
     */
    CylindricJointInfo(Scalar<SI::Density> density, const Transform<SI::Length>& cylinderTransform, bool generateCylinder,
                       const Transform<SI::Length>& socketTransform, Scalar<SI::Length> radius, Scalar<SI::Length> length,
                       Scalar<SI::Angle> startRotation, Scalar<SI::Angle> minAngle, Scalar<SI::Angle> maxAngle,
                       Scalar<SI::Torque> maxMotorTorque, Scalar<SI::AngularFrictionCoefficient> frictionCoefficient) :
        JointInfo(density, cylinderTransform, generateCylinder, socketTransform),
        cylinderRadius(radius),
        cylinderLength(length),
        startRotation(startRotation),
        minAngle(minAngle),
        maxAngle(maxAngle),
        maxMotorTorque(maxMotorTorque),
        frictionCoefficient(frictionCoefficient)
    {

    }

    /** Radius of the cylinder (m). */
    Scalar<SI::Length> cylinderRadius;
    /** Length of the cylinder (m). */
    Scalar<SI::Length> cylinderLength;
    /** Relative orientation of the cylinder body part to the socket part (radian). */
    Scalar<SI::Angle> startRotation;
    /** Minimum possible value for the angle. */
    Scalar<SI::Angle> minAngle;
    /** Maximum possible value for the angle. */
    Scalar<SI::Angle> maxAngle;
    /** Maximum torque (absolute value) that the motor of this joint can apply. */
    Scalar<SI::Torque> maxMotorTorque;
    /** Friction coefficient on the joint axis. */
    Scalar<SI::AngularFrictionCoefficient> frictionCoefficient;

    void addConvexShape(std::vector<CompoundShape::ChildInfo>& shapeInfo) const override;

    std::unique_ptr<Joint> makeJoint(Body& convexPart, Body& concavePart, bool placeConvex) const override;

    /**
     * Creates a CylindricJointInfo object from the content of a Lua table.
     * @param table Lua table from which the object will be constructed.
     * @return The new CylindricJointInfo object.
     */
    static std::unique_ptr<CylindricJointInfo> luaGetFromTable(LuaTable& table);
};

#endif /* CYLINDRICJOINTINFO_HPP */
