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
#include <cmath>

#include "Action.hpp"
#include "Camera.hpp"
#include "irrlicht_ptr.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/types/LuaMethod.hpp"

class CameraAnimator : public irr::scene::ISceneNodeAnimator {
public:
    void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs) override {
        irr::scene::ICameraSceneNode& camera = *static_cast<irr::scene::ICameraSceneNode*>(node);
        float deltaTime = (timeMs - lastTime)/1000.0;
        bool changed = move(camera, deltaTime);
        changed = rotate(camera, deltaTime) || changed;
        if (changed) {
            camera.updateAbsolutePosition();
            irr::scene::ISceneNode& target = **(camera.getChildren().begin());
            target.updateAbsolutePosition();
            camera.setTarget(target.getAbsolutePosition());
        }
        lastTime = timeMs;
    }

    ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager) override {
        return new CameraAnimator(*this);
    }

    /**
     * Creates a new CameraAnimator.
     *
     * @param[in] inputHandler Object used to read bindings & inputs of the GUI.
     */
    CameraAnimator(const InputHandler& inputHandler) : lastTime(0), inputs(inputHandler) {

    }
private:
    /** Time at which the last frame was rendered. */
    irr::u32 lastTime;
    /** Object used to read bindings & inputs of the GUI. */
    const InputHandler& inputs;

    /**
     * Translate the camera according to the current inputs & bindings.
     * @param camera Camera to translate.
     * @param deltaTime Ellapsed time since the last frame was rendered.
     * @return True if the camera was moved.
     */
    bool move(irr::scene::ICameraSceneNode& camera, float deltaTime) {
        /** Translation speed on each axis (in meters/second). */
        static const float translationSpeed = 5.0f;

        irr::core::vector3df translation = {0,0,0};
        float translationOffset = translationSpeed * deltaTime;
        if (inputs.happened(Action::CameraForward)) {
            translation.Z += translationOffset;
        }
        if (inputs.happened(Action::CameraBackward)) {
            translation.Z -= translationOffset;
        }
        if (inputs.happened(Action::CameraLeft)) {
            translation.X -= translationOffset;
        }
        if (inputs.happened(Action::CameraRight)) {
            translation.X += translationOffset;
        }
        if (inputs.happened(Action::CameraUp)) {
            translation.Y += translationOffset;
        }
        if (inputs.happened(Action::CameraDown)) {
            translation.Y -= translationOffset;
        }

        bool result = false;
        if (translation != irr::core::vector3df(0,0,0)) {
            result = true;
            camera.getViewMatrix().inverseRotateVect(translation);
            camera.setPosition(camera.getPosition() + translation);
        }
        return result;
    }

    /**
     * Rotate the camera according to the current inputs & bindings.
     * @param camera Camera to rotate.
     * @param deltaTime Ellapsed time since the last frame was rendered.
     * @return True if the camera was turned.
     */
    bool rotate(irr::scene::ICameraSceneNode& camera, float deltaTime) {
        bool result = false;
        float yawSpeed = 0;
        float pitchSpeed = 0;
        /** Rotation speed of the camera (in degree/sec/inputIntensity). */
        static const float rotationSpeed = 30.0f;

        EventReport event;
        event = inputs.actionReport(Action::CameraTurnRight);
        if (event.happened) {
            yawSpeed+=rotationSpeed*event.intensity;
        }
        event = inputs.actionReport(Action::CameraTurnLeft);
        if (event.happened) {
            yawSpeed-=rotationSpeed*event.intensity;
        }
        event = inputs.actionReport(Action::CameraTurnDown);
        if (event.happened) {
            pitchSpeed+=rotationSpeed*event.intensity;
        }
        event = inputs.actionReport(Action::CameraTurnUp);
        if (event.happened) {
            pitchSpeed-=rotationSpeed*event.intensity;
        }
        if (yawSpeed != 0 || pitchSpeed != 0) {
            result = true;
            irr::core::vector3df rotation(camera.getRotation());
            rotation.X= std::clamp<float>(rotation.X + pitchSpeed*deltaTime, -89.0f, 89.0f);
            rotation.Y= std::remainder(rotation.Y + yawSpeed*deltaTime, 360.0f);
            camera.setRotation(rotation);
        }
        return result;
    }
};

Camera::Camera(irr::scene::ISceneManager& scene, const InputHandler& inputHandler) :
    camera(scene.addCameraSceneNode(nullptr, irr::core::vector3df(0, 0, 0), irr::core::vector3df(0, 0, -1))),
    target(*scene.addEmptySceneNode(camera.get()))
{
    irrlicht_ptr<CameraAnimator> animator(new CameraAnimator(inputHandler));
    camera->addAnimator(animator.get());
    target.setPosition(irr::core::vector3df(0,0,1));
    camera->setTarget(target.getAbsolutePosition());
}

void Camera::setPosition(const irr::core::vector3df& pos) {
    camera->setPosition(pos);
    camera->updateAbsolutePosition();
    target.updateAbsolutePosition();
    camera->setTarget(target.getAbsolutePosition());
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
