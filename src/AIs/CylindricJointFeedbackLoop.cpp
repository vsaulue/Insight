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

#include <algorithm>

#include "CylindricJointFeedbackLoop.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaMethod.hpp"

CylindricJointFeedbackLoop::CylindricJointFeedbackLoop(const Sense<float>& sense, Action<float>& action) :
    inputRotation(sense),
    outputMotorTorque(action),
    targetAngle(0),
    previousAngle(inputRotation.get())
{

}

CylindricJointFeedbackLoop::~CylindricJointFeedbackLoop() = default;

void CylindricJointFeedbackLoop::stepSimulation() {
    float newAngle = inputRotation.get();
    float delta = newAngle - targetAngle;
    float speed = newAngle - previousAngle;
    outputMotorTorque.set(-speed-0.2*delta);
    previousAngle = newAngle;
}

int CylindricJointFeedbackLoop::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<CylindricJointFeedbackLoop>;
    int result = 1;
    if (memberName == "target") {
        state.push<float>(targetAngle);
    } else if (memberName == "setTarget") {
        state.push<Method>([](CylindricJointFeedbackLoop& object, LuaStateView& state) -> int {
            auto value = state.get<float>(2);
            object.targetAngle = std::clamp(value, -SIMD_PI, SIMD_PI);
            return 0;
        });
    } else {
        result = 0;
    }
    return result;
}
