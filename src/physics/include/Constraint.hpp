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

#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP

#include "btBulletDynamicsCommon.h"

#include "units/Scalar.hpp"
#include "units/BulletUnits.hpp"

class World;

/** Generic constraint in the physics engine. */
class Constraint {
public:
    virtual ~Constraint() = default;

    /**
     * Gets the Bullet constraint implementing this joint.
     * @return The constraint implementing this joint.
     */
    virtual btTypedConstraint& getConstraint() = 0;

    /**
     * Method called by the World containing this object before each integration step.
     *
     * @param world World containing this constraint.
     * @param timeStep Duration of the next integration step.
     */
    virtual void beforeTick(World& world, Scalar<BulletUnits::Time> timeStep) = 0;
};

#endif /* CONSTRAINT_HPP */
