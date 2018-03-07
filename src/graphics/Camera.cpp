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

#include "Action.hpp"
#include "Camera.hpp"
#include "irrlicht_ptr.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/types/LuaMethod.hpp"

class CameraAnimator : public irr::scene::ISceneNodeAnimator {
public:
    void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs) override {
        using irr::core::vector3df;
        /** Translation speed on each axis (in meters/second). */
        static const float translationSpeed = 5.0f;

        irr::scene::ICameraSceneNode& camera = *static_cast<irr::scene::ICameraSceneNode*>(node);

        vector3df translation = {0,0,0};
        float translationOffset = translationSpeed * (timeMs-lastTime) / 1000;
        if (inputs.isKeyDown(Action::CameraForward)) {
            translation.Z += translationOffset;
        }
        if (inputs.isKeyDown(Action::CameraBackward)) {
            translation.Z -= translationOffset;
        }
        if (inputs.isKeyDown(Action::CameraLeft)) {
            translation.X -= translationOffset;
        }
        if (inputs.isKeyDown(Action::CameraRight)) {
            translation.X += translationOffset;
        }
        if (inputs.isKeyDown(Action::CameraUp)) {
            translation.Y += translationOffset;
        }
        if (inputs.isKeyDown(Action::CameraDown)) {
            translation.Y -= translationOffset;
        }

        if (translation != irr::core::vector3df(0,0,0)) {
            camera.getViewMatrix().inverseRotateVect(translation);
            camera.setPosition(node->getPosition() + translation);
            camera.setTarget(camera.getTarget() + translation);
        }
        lastTime = timeMs;
    }

    ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager) override {
        return new CameraAnimator(*this);
    }

    CameraAnimator(const InputHandler& inputHandler) : lastTime(0), inputs(inputHandler) {

    }
private:
    irr::u32 lastTime;
    const InputHandler& inputs;
};

Camera::Camera(irr::scene::ISceneManager& scene, const InputHandler& inputHandler) :
    camera(scene.addCameraSceneNode(nullptr, irr::core::vector3df(0, 0, 0), irr::core::vector3df(0, 0, -1)))
{
    irrlicht_ptr<CameraAnimator> animator(new CameraAnimator(inputHandler));
    camera->addAnimator(animator.get());
}

void Camera::setPosition(const irr::core::vector3df& pos) {
    irr::core::vector3df translation = pos - camera->getPosition();
    camera->setPosition(pos);
    camera->setTarget(camera->getTarget()+translation);
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
            object.setPosition(pos);
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
