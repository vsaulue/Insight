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

#include "CylindricJointInfo.hpp"
#include "JointInfo.hpp"
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/std/string.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "SphericalJointInfo.hpp"

int JointInfo::luaIndex(const std::string& memberName, LuaStateView& state) {
    return 0;
}

std::unique_ptr<JointInfo> JointInfo::luaGetFromTable(LuaTable& table) {
    std::string type = table.get<LuaNativeString,std::string>("type");
    LuaTable params = table.get<LuaNativeString,LuaTable>("params");
    std::unique_ptr<JointInfo> result;
    if (type == "Spherical") {
        result = SphericalJointInfo::luaGetFromTable(params);
    } else if (type == "Cylindric") {
        result = CylindricJointInfo::luaGetFromTable(params);
    } else {
        std::string msg = std::string("Invalid 'type' field in Shape table constructor: ") + type;
        throw LuaException(msg.c_str());
    }
    return result;
}
