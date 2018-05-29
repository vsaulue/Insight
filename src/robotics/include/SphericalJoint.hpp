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

#include "Body.hpp"
#include "CompoundShape.hpp"
#include "Joint.hpp"
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

    std::shared_ptr<btTypedConstraint> getConstraint() override {
        return constraint;
    }
private:
    /** Joint configuration. */
    const SphericalJointInfo& jointInfo;
    /** Bullet constraint implementing this joint.*/
    std::shared_ptr<btConeTwistConstraint> constraint;
};

#endif /* JOINT_HPP */
