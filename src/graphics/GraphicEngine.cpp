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


#include <cstdlib>

#include "btBulletDynamicsCommon.h"

#include "GraphicEngine.hpp"

using namespace irr;

GraphicEngine::GraphicEngine(const World& world) : world(world) {

    device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(1024, 768), 16, false, false, true, NULL);

    sceneManager = device->getSceneManager();
    guienv = device->getGUIEnvironment();
    driver = device->getVideoDriver();

    device->setWindowCaption(L"Insight - Renderer");
    sceneManager->addCameraSceneNode(NULL, core::vector3df(0, 0, 10), core::vector3df(0, 0, -1));

    monkey = sceneManager->getMesh("monkey.obj");
    /* scene::IAnimatedMeshSceneNode *node = */
    //sceneManager->addAnimatedMeshSceneNode(monkey);

    scene::ILightSceneNode *light = sceneManager->addLightSceneNode(NULL, core::vector3df(2.0f, 2.0f, 2.0f), video::SColorf(1.0f, 1.0f, 1.0f));

    light->setLightType(video::ELT_DIRECTIONAL);
    light->setRotation(core::vector3df(135.0f, .0f, 45.0f));

    video::SLight& lightData = light->getLightData();
    lightData.Direction = core::vector3df(-.5f, -.5f, -.5f);
    lightData.DiffuseColor = video::SColorf(.6f, .6f, .6f);
    lightData.AmbientColor = video::SColorf(.2f, .2f, .2f);
}

GraphicEngine::~GraphicEngine() {
    device->drop();
}

void GraphicEngine::doRender() {
    driver->beginScene(true, true, video::SColor(255, 0, 0, 0));

    for (auto it = world.begin(); it != world.end(); ++it) {

        auto map_it = mapping.find((*it).get());
        scene::IAnimatedMeshSceneNode *node;
        if (map_it == mapping.end()) {
            node = sceneManager->addAnimatedMeshSceneNode(monkey);
            mapping[(*it).get()] = node;
        } else {
            node = (*map_it).second;
        }

        const btVector3& pos = (*it)->getPosition();
        node->setPosition(core::vector3df(pos.getX(), pos.getY(), pos.getZ()));
    }
    sceneManager->drawAll();
    guienv->drawAll();

    driver->endScene();
}

bool GraphicEngine::run() {
    return device->run();
}
