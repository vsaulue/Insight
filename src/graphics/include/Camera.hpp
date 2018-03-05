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

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <memory>

#include "irrlicht.h"
#include "lua/types/LuaVirtualClass.hpp"

/**
 * Camera of the graphics engine.
 */
class Camera : public LuaVirtualClass {
public:
    /**
     * Creates a new camera.
     *
     * @param scene Scene in which the camera is created.
     */
    Camera(irr::scene::ISceneManager& scene);

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
private:
    /** Custom deleter for a camera scene node (for unique_ptr). */
    struct CameraDeleter {
    public:
        void operator()(irr::scene::ICameraSceneNode* camera) {
            camera->remove();
        }
    };

    /** Scene node of the camera in Irrlicht. */
    std::unique_ptr<irr::scene::ICameraSceneNode, CameraDeleter> camera;
};

#endif /* CAMERA_HPP */
