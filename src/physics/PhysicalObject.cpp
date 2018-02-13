/* This file is part of Insight.
 * Copyright (C) 2017 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#include <string.h>
#include <memory.h>

#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/types/LuaMethod.hpp"

#include "PhysicalObject.hpp"

PhysicalObject::PhysicalObject() {
    position[0] = position[1] = position[2] = 0.0;
    velocity[0] = velocity[1] = velocity[2] = 0.0;
}

int PhysicalObject::luaIndex(const std::string& memberName, LuaStateView& state) {
    int result = 0;
    if (memberName=="setPosition") {
        state.push<LuaMethod<PhysicalObject>>([](PhysicalObject& object, LuaStateView& state) -> int {
            double x = state.get<double>(2);
            double y = state.get<double>(3);
            double z = state.get<double>(4);
            object.position[0] = x;
            object.position[1] = y;
            object.position[2] = z;
            return 0;
        });
        result = 1;
    } else if (memberName=="setVelocity") {
        state.push<LuaMethod<PhysicalObject>>([](PhysicalObject& object, LuaStateView& state) -> int {
            double x = state.get<double>(2);
            double y = state.get<double>(3);
            double z = state.get<double>(4);
            object.velocity[0] = x;
            object.velocity[1] = y;
            object.velocity[2] = z;
            return 0;
        });
        result = 1;
    } else if (memberName=="position") {
        state.push<LuaMethod<PhysicalObject>>([](PhysicalObject& object, LuaStateView& state) -> int {
            state.push<double>(object.position[0].toDouble());
            state.push<double>(object.position[1].toDouble());
            state.push<double>(object.position[2].toDouble());
            return 3;
        });
        result = 1;
    } else if (memberName=="velocity") {
        state.push<LuaMethod<PhysicalObject>>([](PhysicalObject& object, LuaStateView& state) -> int {
            state.push<double>(object.velocity[0]);
            state.push<double>(object.velocity[1]);
            state.push<double>(object.velocity[2]);
            return 3;
        });
        result = 1;
    }
    return result;
}
