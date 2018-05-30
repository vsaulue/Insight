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

#ifndef LUA_BINDINGS_ROBOTICS_HPP
#define LUA_BINDINGS_ROBOTICS_HPP

#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/types/LuaTable.hpp"
#include "RobotBody.hpp"

template<>
class LuaBinding<RobotBody::ConstructionInfo> : public LuaDefaultBinding<RobotBody::ConstructionInfo> {
public:
    using ConstructionInfo = RobotBody::ConstructionInfo;
    using JointInputData = ConstructionInfo::JointInputData;

    static ConstructionInfo getFromTable(LuaTable& table);
};

template<>
class LuaBinding<RobotBody::ConstructionInfo::JointInputData> : public LuaDefaultBinding<RobotBody::ConstructionInfo::JointInputData> {
public:
    using ConstructionInfo = RobotBody::ConstructionInfo;
    using JointInputData = ConstructionInfo::JointInputData;

    static JointInputData getFromTable(LuaTable& table);
};

#endif /* LUA_BINDINGS_ROBOTICS_HPP */
