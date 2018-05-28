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

#ifndef JOINTINFO_HPP
#define JOINTINFO_HPP

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "Body.hpp"
#include "CompoundShape.hpp"
#include "Joint.hpp"

/** Joint configuration info. */
struct JointInfo {
public:
    /** No initialisation constructor. */
    JointInfo() = default;

    /**
     * Full initialisation constructor.
     * @param density Density of the generated body parts.
     * @param placeConvex True if the convex body should be moved to match the position of the concave body. False otherwise.
     * @param convexTransform Relative transform of the convex joint component in its body part.
     * @param generateConvex True if the convex shape should be added to the shape of its body.
     * @param concaveTransform Relative transform of the concave joint component in its body part.
     */
    JointInfo(btScalar density, bool placeConvex, const btTransform& convexTransform, bool generateConvex, const btTransform& concaveTransform) :
        jointDensity(density),
        placeConvex(placeConvex),
        convexTransform(convexTransform),
        generateConvexShape(generateConvex),
        concaveTransform(concaveTransform)
    {

    }

    virtual ~JointInfo() = default;

    /**
     * Adds the convex shape of the joint to the corresponding CompoundShape.
     * @param shapeInfo Construction info of the CompoundShape holding the convex part of the joint.
     */
    virtual void addConvexShape(std::vector<CompoundShape::ChildInfo>& shapeInfo) const = 0;

    /**
     * Creates a new joint from this construction info.
     * @param convexPart Body containing the convex part of the joint.
     * @param concavePart Body containing the concave part of the joint.
     */
    virtual std::unique_ptr<Joint> makeJoint(Body& convexPart, Body& concavePart) const = 0;

    /** Density of the generated parts (kg/m^3). */
    btScalar jointDensity;
    /** True to place the convex part according to the position of the concave part. False otherwise. */
    bool placeConvex;
    /** Relative position of the convex joint part to the CompoundBody containing it. */
    btTransform convexTransform;
    /** Generates the collision shape of the convex part. */
    bool generateConvexShape;
    /**
     * Relative position of the concave part to the CompoundBody containing it.
     *
     * The socket position/origin is the center of the associated convex part.
     */
    btTransform concaveTransform;
};

#endif /* JOINTINFO_HPP */
