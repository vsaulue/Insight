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
#include "btConversions.hpp"
#include "Camera.hpp"
#include "InputSettings.hpp"
#include "irrlicht_ptr.hpp"
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/types/LuaMethod.hpp"
#include "units/IrrUnits.hpp"
#include "units/Vector3.hpp"

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
        float translationSpeed = toIrrUnits(inputs.getSettings().cameraTranslationSpeed);

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
        float rotationSpeed = toIrrUnits(inputs.getSettings().cameraRotationSpeed);

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
            rotation.X= std::remainder(rotation.X, 360.0f);
            rotation.X= std::clamp<float>(rotation.X + pitchSpeed*deltaTime, -89.0f, 89.0f);
            rotation.Y= std::remainder(rotation.Y + yawSpeed*deltaTime, 360.0f);
            camera.setRotation(rotation);
        }
        return result;
    }
};

Camera::Camera(irr::scene::ISceneManager& scene, const InputHandler& inputHandler) :
    camera(scene.addCameraSceneNode(nullptr)),
    target(*scene.addEmptySceneNode(camera.get()))
{
    irrlicht_ptr<CameraAnimator> animator(new CameraAnimator(inputHandler));
    camera->bindTargetAndRotation(true);
    camera->addAnimator(animator.get());
    target.setPosition(irr::core::vector3df(0,0,1));
    setPosition(irr::core::vector3df(0,0,0));
}

void Camera::setPosition(const irr::core::vector3df& pos) {
    camera->setPosition(pos);
    camera->updateAbsolutePosition();
    target.updateAbsolutePosition();
    camera->setTarget(target.getAbsolutePosition());
}

int Camera::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<Camera>;
    int result = 1;
    if (memberName == "setPosition") {
        state.push<Method>([](Camera& object, LuaStateView& state) -> int {
            auto pos = state.get<Vector3<SI::Length>>(2);
            object.setPosition(btToIrrVector(toIrrUnits(pos)));
            return 0;
        });
    } else if (memberName == "position") {
        const irr::core::vector3df& rawPos = camera->getPosition();
        auto pos = fromIrrValue<SI::Length>(irrToBtVector(rawPos));
        state.push<Vector3<SI::Length>>(pos);
    } else if (memberName == "lookAt") {
        state.push<Method>([](Camera& object, LuaStateView& state) -> int {
            auto pos = state.get<Vector3<SI::Length>>(2);
            object.camera->setTarget(btToIrrVector(toIrrUnits(pos)));
            return 0;
        });
    } else {
        result = 0;
    }
    return result;
}
