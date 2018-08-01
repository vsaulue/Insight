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

#ifndef CYLINDRICJOINT_HPP
#define CYLINDRICJOINT_HPP

#include <memory>

#include "Body.hpp"
#include "CylindricJointInfo.hpp"
#include "Joint.hpp"
#include "Sense.hpp"
#include "units/Scalar.hpp"
#include "units/SI.hpp"

/**
 * Single degree of freedom joint in a RobotBody.
 *
 * A cylindric joint links two CompoundBody objects called:
 * <ul>
 * <li>the cylinder part: containing the convex (cylinder) part of the joint.</li>
 * <li>the socket part: containing the concave part of the joint.</li>
 * </ul>
 */
class CylindricJoint : public Joint {
public:
    /**
     * Creates a new cylindric joint between two body parts.
     *
     * @param cylinder Body part containing the cylindric part of the joint.
     * @param socket Body part containing the concave part of the joint.
     * @param info Configuration of this joint.
     * @param bool placeCylinder True to move the cylinder body to match the socket body. false otherwise.
     */
    CylindricJoint(Body& cylinder, Body& socket, const CylindricJointInfo& info, bool placeCylinder);

    btTypedConstraint& getConstraint() override {
        return constraint;
    }

    void beforeTick(World& world, Scalar<BulletUnits::Time> timeStep) override;

    virtual ~CylindricJoint();

    /**
     * Gets the relative rotation of the cylinder in the socket frame.
     * @return The relative rotation between the two parts.
     */
    Scalar<SI::Angle> getRotation();

    SenseSignal& getRotationSense() override;
private:
    /** Joint configuration. */
    const CylindricJointInfo& jointInfo;
    /** Bullet constraint implementing this joint.*/
    btHingeConstraint constraint;
    /** Sense returning the relative rotation of the cylinder in the socket frame. */
    Sense<float> rotationSense;
};

#endif /* CYLINDRICJOINT_HPP */
