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

GraphicEngine::GraphicEngine(const World& world) :
    device(createDevice(video::EDT_OPENGL, core::dimension2d<u32>(1024, 768), 16, false, false, true, NULL)),
    camera(*device->getSceneManager()),
    world(world)
{
    sceneManager = device->getSceneManager();
    guienv = device->getGUIEnvironment();
    driver = device->getVideoDriver();

    device->setWindowCaption(L"Insight - Renderer");

    scene::ILightSceneNode *light = sceneManager->addLightSceneNode(NULL, core::vector3df(2.0f, 2.0f, 2.0f), video::SColorf(1.0f, 1.0f, 1.0f));

    light->setLightType(video::ELT_DIRECTIONAL);
    light->setRotation(core::vector3df(135.0f, .0f, 45.0f));

    video::SLight& lightData = light->getLightData();
    lightData.Direction = core::vector3df(-.5f, -.5f, -.5f);
    lightData.DiffuseColor = video::SColorf(.6f, .6f, .6f);
    lightData.AmbientColor = video::SColorf(.2f, .2f, .2f);
}

void GraphicEngine::doRender() {
    driver->beginScene(true, true, video::SColor(255, 0, 0, 0));

    for (auto it = world.begin(); it != world.end(); ++it) {
        const Body& body = *it->get();

        auto map_it = mapping.find(&body);
        GraphicObject* object;
        if (map_it == mapping.end()) {
            std::unique_ptr<GraphicObject> ptr = std::make_unique<GraphicObject>(body, sceneManager);
            object = ptr.get();
            mapping[(*it).get()] = std::move(ptr);
        } else {
            object = (*map_it).second.get();
        }

        const btVector3& pos = body.getPosition();
        object->setPosition(pos);
    }
    sceneManager->drawAll();
    guienv->drawAll();

    driver->endScene();
}

bool GraphicEngine::run() {
    return device->run();
}

int GraphicEngine::luaIndex(const std::string& memberName, LuaStateView& state) {
    if (memberName=="camera") {
        state.push<Camera*>(&camera);
        return 1;
    }
    return 0;
}
