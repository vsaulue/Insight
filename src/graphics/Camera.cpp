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

#include "Camera.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/types/LuaMethod.hpp"

Camera::Camera(irr::scene::ISceneManager& scene) :
    camera(scene.addCameraSceneNode(nullptr, irr::core::vector3df(0, 0, 10), irr::core::vector3df(0, 0, -1)))
{

}

/**
 * Reads a vector3df from Lua stack.
 *
 * @param state State containing the stack.
 * @param firstIndex Stack index of the first coordinate of the vector.
 *
 * @return The vector read from the Lua stack.
 */
static irr::core::vector3df readVector3df(LuaStateView& state, int firstIndex) {
    float x = state.get<float>(firstIndex);
    float y = state.get<float>(firstIndex+1);
    float z = state.get<float>(firstIndex+2);
    return irr::core::vector3df(x,y,z);
}

int Camera::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<Camera>;
    if (memberName == "setPosition") {
        state.push<Method>([](Camera& object, LuaStateView& state) -> int {
            irr::core::vector3df pos = readVector3df(state, 2);
            object.camera->setPosition(pos);
            return 0;
        });
        return 1;
    } else if (memberName == "position") {
        state.push<Method>([](Camera& object, LuaStateView& state) -> int {
            const irr::core::vector3df& pos = object.camera->getPosition();
            state.push<float>(pos.X);
            state.push<float>(pos.Y);
            state.push<float>(pos.Z);
            return 3;
        });
        return 1;
    } else if (memberName == "lookAt") {
        state.push<Method>([](Camera& object, LuaStateView& state) -> int {
            irr::core::vector3df pos = readVector3df(state, 2);
            object.camera->setTarget(pos);
            return 0;
        });
        return 1;
    }
    return 0;
}
