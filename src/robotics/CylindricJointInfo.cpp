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

#include "CylinderShape.hpp"
#include "CylindricJoint.hpp"
#include "CylindricJointInfo.hpp"
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaNativeString.hpp"

void CylindricJointInfo::addConvexShape(std::vector<CompoundShape::ChildInfo>& info) const {
    if (generateConvexShape) {
        Vector3<SI::Length> halfExtents(cylinderRadius, cylinderLength/2, cylinderRadius);
        Transform<SI::Length> transform = convexTransform * Transform<SI::Length>(btQuaternion(btVector3(0,0,1), SIMD_HALF_PI));
        info.push_back({std::make_shared<CylinderShape>(jointDensity, halfExtents), transform});
    }
}

std::unique_ptr<Joint> CylindricJointInfo::makeJoint(Body& convexPart, Body& concavePart, bool placeConvex) const {
    return std::make_unique<CylindricJoint>(convexPart, concavePart, *this, placeConvex);
}

std::unique_ptr<CylindricJointInfo> CylindricJointInfo::luaGetFromTable(LuaTable& table) {
    return std::make_unique<CylindricJointInfo>(
            table.get<LuaNativeString,Scalar<SI::Density>>("density"),
            table.get<LuaNativeString,Transform<SI::Length>>("convexTransform"),
            table.get<LuaNativeString,bool>("generateConvexShape"),
            table.get<LuaNativeString,Transform<SI::Length>>("concaveTransform"),
            table.get<LuaNativeString,Scalar<SI::Length>>("radius"),
            table.get<LuaNativeString,Scalar<SI::Length>>("length"),
            table.get<LuaNativeString,Scalar<SI::Angle>>("startRotation"),
            table.get<LuaNativeString,Scalar<SI::Angle>>("minAngle"),
            table.get<LuaNativeString,Scalar<SI::Angle>>("maxAngle"),
            table.get<LuaNativeString,Scalar<SI::Torque>>("maxMotorTorque")
    );
}
