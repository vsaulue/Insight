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

#ifndef GRAPHICENGINE_HPP
#define	GRAPHICENGINE_HPP

#include <irrlicht.h>
#include <unordered_map>

#include "World.hpp"

class GraphicEngine {
private:
    irr::IrrlichtDevice *device;
    irr::scene::ISceneManager *sceneManager;
    irr::gui::IGUIEnvironment *guienv;
    irr::video::IVideoDriver *driver;
    irr::scene::IAnimatedMesh *monkey;

    const World& world;
    std::unordered_map<const Body*, irr::scene::IAnimatedMeshSceneNode*> mapping;

public:

    GraphicEngine(const World& world);

    virtual ~GraphicEngine();

    void doRender();

    bool run();
};

#endif	/* GRAPHICENGINE_HPP */

