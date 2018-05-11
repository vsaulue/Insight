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

#include <vector>

#include "btBulletDynamicsCommon.h"

#include "CompoundShape.hpp"

/**
 * Parameters to describe & construct a spherical joint.
 */
struct SphericalJointInfo {
public:
    /** Radius of the ball (m).*/
    btScalar ballRadius;
    /** Relative orientation of the ball body part to the socket part. */
    btQuaternion startRotation;
    /** Density of the generated parts (kg/m^3). */
    btScalar jointDensity;
    /**
     * Tells which body part should be moved to match the position of the other part.
     *
     * True if the ball part should be placed in the socket part.
     * False will place the socket according to the current position of the ball. */
    bool placeBall;
    /** Relative position of the ball to the CompoundBody containing it. */
    btTransform ballTransform;
    /** Generates the collision shape of the ball. */
    bool generateBallShape;
    /**
     * Relative position of the socket to the CompoundBody containing it.
     *
     * The socket position/origin is the center of the associated ball.
     */
    btTransform socketTransform;

    void addSphereShape(std::vector<CompoundShape::ChildInfo>& shapeInfo) const;
};

#endif /* SPHERICALJOINTINFO_HPP */
