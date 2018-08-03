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

#include "SphereShape.hpp"
#include "SphericalJoint.hpp"
#include "SphericalJointInfo.hpp"
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaNativeString.hpp"

void SphericalJointInfo::addConvexShape(std::vector<CompoundShape::ChildInfo>& shapeInfo) const {
    if (generateConvexShape) {
        shapeInfo.push_back({std::make_shared<SphereShape>(jointDensity, ballRadius), convexTransform});
    }
}

std::unique_ptr<Joint> SphericalJointInfo::makeJoint(Body& convexPart, Body& concavePart, bool placeConvex) const {
    return std::make_unique<SphericalJoint>(convexPart, concavePart, *this, placeConvex);
}

std::unique_ptr<SphericalJointInfo> SphericalJointInfo::luaGetFromTable(LuaTable& table) {
    return std::make_unique<SphericalJointInfo>(
            table.get<LuaNativeString,Scalar<SI::Density>>("density"),
            table.get<LuaNativeString,Transform<SI::Length>>("convexTransform"),
            table.get<LuaNativeString,bool>("generateConvexShape"),
            table.get<LuaNativeString,Transform<SI::Length>>("concaveTransform"),
            table.get<LuaNativeString,Scalar<SI::Length>>("radius"),
            table.get<LuaNativeString,btQuaternion>("startRotation"),
            table.get<LuaNativeString,Vector3<SI::Angle>>("limits"),
            table.get<LuaNativeString,Vector3<SI::Torque>>("maxMotorTorque")
    );
}
