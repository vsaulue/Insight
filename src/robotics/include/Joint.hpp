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

#ifndef JOINT_HPP
#define JOINT_HPP

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "Body.hpp"
#include "Constraint.hpp"
#include "SenseSignal.hpp"

/** Common interface for all joint types. */
class Joint : public Constraint {
protected:
    /** Body part holding the convex part (ex: sphere, cylinder) of the joint. */
    Body& convexPart;
    /** Body part holding the concave part of the joint. */
    Body& concavePart;
public:
    /**
     * Creates a new Joint.
     * @param convexPart Body part containing the convex part of the joint.
     * @param concavePart Body part containing the concave part of the joint.
     */
    Joint(Body& convexPart, Body& concavePart) : convexPart(convexPart), concavePart(concavePart) {}

    Joint(const Joint&) = delete;
    Joint(Joint&&) = delete;

    /**
     * Gets the body part containing the convex part of the joint.
     * @return The convex part of the joint.
     */
    Body& getConvexPart() {
        return convexPart;
    }

    /**
     * Gets the body part containing the concave part of the joint.
     * @return The concave part of the joint.
     */
    Body& getConcavePart() {
        return concavePart;
    }

    virtual ~Joint() = default;

    /**
     * Gets the sense returning the relative rotation of the two parts (proprioception).
     * @return A sense returning the relative transform of the convex part relative to the concave part.
     */
    virtual SenseSignal& getRotationSense() = 0;
};

#endif /* JOINT_HPP */
