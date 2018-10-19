/* This file is part of Insight.
 * Copyright (C) 2017-2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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


#include <cstdlib>

#include "btBulletDynamicsCommon.h"

#include "GraphicEngine.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"

using namespace irr;

void GraphicEngine::addBody(const Body& body) {
    std::unique_ptr<GraphicObject> ptr = std::make_unique<GraphicObject>(body, sceneManager);
    mapping[&body] = std::move(ptr);
}

/**
 * Creates the parameter for irr::createDeviceEx.
 * @return The construction info for the Irrlicht context.
 */
static SIrrlichtCreationParameters getIrrlichtCreationParameters() {
    SIrrlichtCreationParameters result;
    result.DriverType = video::EDT_OPENGL;
    result.WindowSize = core::dimension2d<u32>(1024, 768);
    result.LoggingLevel = ELL_WARNING;
    return result;
}

GraphicEngine::GraphicEngine(const World& world) :
    device(createDeviceEx(getIrrlichtCreationParameters())),
    sceneManager(*device->getSceneManager()),
    guienv(*device->getGUIEnvironment()),
    driver(*device->getVideoDriver()),
    inputs(*device),
    camera(sceneManager, inputs),
    world(world)
{
    device->setEventReceiver(&inputs);
    device->setWindowCaption(L"Insight - Renderer");

    scene::ILightSceneNode *light = sceneManager.addLightSceneNode(NULL, core::vector3df(2.0f, 2.0f, 2.0f), video::SColorf(1.0f, 1.0f, 1.0f));

    light->setLightType(video::ELT_DIRECTIONAL);
    light->setRotation(core::vector3df(135.0f, .0f, 45.0f));

    video::SLight& lightData = light->getLightData();
    lightData.Direction = core::vector3df(-.5f, -.5f, -.5f);
    lightData.DiffuseColor = video::SColorf(.6f, .6f, .6f);
    lightData.AmbientColor = video::SColorf(.2f, .2f, .2f);

    for (auto& body : world) {
        addBody(*body.get());
    }
    world.addCreationListener(*this);
}

void GraphicEngine::run() {
    inputs.newFrame();
    if(device->run()) {
        inputs.doActions();
        driver.beginScene(true, true, video::SColor(255, 0, 0, 0));
        sceneManager.drawAll();
        guienv.drawAll();
        driver.endScene();
    }
}

void GraphicEngine::onBodyCreation(const Body& newBody) {
    addBody(newBody);
}


int GraphicEngine::luaIndex(const std::string& memberName, LuaStateView& state) {
    if (memberName=="camera") {
        state.push<Camera*>(&camera);
        return 1;
    } else if (memberName == "inputs") {
        state.push<InputHandler*>(&inputs);
        return 1;
    }
    return 0;
}

GraphicEngine::~GraphicEngine() {
    world.removeCreationListener(*this);
}
