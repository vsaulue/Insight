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

#include "JointInfo.hpp"
#include "lua/bindings/luaVirtualClass/shared_ptr.hpp"
#include "lua/bindings/robotics.hpp"
#include "lua/bindings/std/string.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "Shape.hpp"

using JointInputData = RobotBody::ConstructionInfo::JointInputData;

template<typename K, typename V>
std::unordered_map<K,V> getMap(LuaTable& table, const char* fieldName) {
    LuaTable subTable = table.get<LuaNativeString,LuaTable>(fieldName);
    return subTable.asMap<K,V>();
}

RobotBody::ConstructionInfo LuaBinding<RobotBody::ConstructionInfo>::getFromTable(LuaTable& table) {
    auto parts = getMap<std::string,std::shared_ptr<Shape>>(table, "parts");
    auto base = table.get<LuaNativeString,std::string>("basePart");
    auto joints = getMap<std::string,JointInputData>(table, "joints");
    return ConstructionInfo(parts, base, joints);
}

JointInputData LuaBinding<JointInputData>::getFromTable(LuaTable& table) {
    JointInputData result{
        table.get<LuaNativeString,std::shared_ptr<JointInfo>>("info"),
        table.get<LuaNativeString,std::string>("convexPart"),
        table.get<LuaNativeString,std::string>("concavePart"),
    };
    return result;
}
