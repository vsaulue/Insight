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

#include "Camera.hpp"
#include "GraphicObject.hpp"
#include "InputHandler.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "World.hpp"
#include "irrlicht_ptr.hpp"

class GraphicEngine : public LuaVirtualClass {
private:
    /** Object managing mouse & keyboard inputs. */
    InputHandler inputs;

    /** Irrlicht device. */
    irrlicht_ptr<irr::IrrlichtDevice> device;
    /** Irrlicht scene manager. */
    irr::scene::ISceneManager& sceneManager;
    /** Irrlicht GUI environment. */
    irr::gui::IGUIEnvironment& guienv;
    /** Irrlicht driver. */
    irr::video::IVideoDriver& driver;

    /** Camera in the Irrlicht scene. */
    Camera camera;

    /** Physics world rendered by this GraphicEngine. */
    const World& world;
    /** Map givind the 3d object of a given Body. */
    std::unordered_map<const Body*, std::unique_ptr<GraphicObject>> mapping;

public:
    /**
     * Creates a new graphic engine.
     *
     * @param[in] world World to display.
     */
    GraphicEngine(const World& world);

    /**
     * Render the next frame.
     */
    void doRender();

    /**
     * Run the engine.
     *
     * This function will handle events (mouse clicks, window resize, ...)
     *
     * @return True if the window was not closed.
     */
    bool run();

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
};

#endif	/* GRAPHICENGINE_HPP */

