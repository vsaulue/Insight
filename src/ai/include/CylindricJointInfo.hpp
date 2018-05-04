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

/**
 * Parameters to describe & construct a cylindric joint.
 */
struct CylindricJointInfo {
    /** Radius of the cylinder (m). */
    btScalar cylinderRadius;
    /** Length of the cylinder (m). */
    btScalar cylinderLength;
    /** Relative orientation of the cylinder body part to the socket part. */
    btScalar startRotation;
    /** Density of the generated parts (kg/m^3). */
    btScalar jointDensity;
    /**
     * Tells which body part should be moved to match the position of the other part.
     *
     * True if the cylinder part should be placed in the socket part.
     * False will place the socket according to the current position of the cylinder. */
    bool placeCylinder;
    /** Relative position of the cylinder to the CompoundBody containing it. */
    btTransform cylinderTransform;
    /** Generates the collision shape of the cylinder. */
    bool generateCylinder;
    /**
     * Relative position of the socket to the CompoundBody containing it.
     *
     * The socket position/origin is the center of the associated cylinder.
     */
    btTransform socketTransform;
};

#endif /* CYLINDRICJOINTINFO_HPP */
