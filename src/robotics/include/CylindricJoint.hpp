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
     */
    CylindricJoint(Body& cylinder, Body& socket, const CylindricJointInfo& info);

    std::shared_ptr<btTypedConstraint> getConstraint() override {
        return constraint;
    }

    virtual ~CylindricJoint();
private:
    /** Joint configuration. */
    const CylindricJointInfo& jointInfo;
    /** Bullet constraint implementing this joint.*/
    std::shared_ptr<btHingeConstraint> constraint;
};

#endif /* CYLINDRICJOINT_HPP */
