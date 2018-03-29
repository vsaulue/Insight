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

#include "BodyCreationListener.hpp"
#include "Camera.hpp"
#include "GraphicObject.hpp"
#include "InputHandler.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "World.hpp"
#include "irrlicht_ptr.hpp"

class GraphicEngine : public LuaVirtualClass, public BodyCreationListener {
private:
    /** Irrlicht device. */
    irrlicht_ptr<irr::IrrlichtDevice> device;
    /** Irrlicht scene manager. */
    irr::scene::ISceneManager& sceneManager;
    /** Irrlicht GUI environment. */
    irr::gui::IGUIEnvironment& guienv;
    /** Irrlicht driver. */
    irr::video::IVideoDriver& driver;

    /** Object managing mouse & keyboard inputs. */
    InputHandler inputs;

    /** Camera in the Irrlicht scene. */
    Camera camera;

    /** Physics world rendered by this GraphicEngine. */
    const World& world;
    /** Map givind the 3d object of a given Body. */
    std::unordered_map<const Body*, std::unique_ptr<GraphicObject>> mapping;

    /**
     * Adds a new GraphicObject representing an object in the physics engine.
     *
     * @param[in] body Object in the physics engine to represent.
     */
    void addBody(const Body& body);
public:
    /**
     * Creates a new graphic engine.
     *
     * @param[in] world World to display.
     */
    GraphicEngine(const World& world);

    /**
     * Run the engine.
     *
     * This function will handle events (mouse clicks, window resize, ...), and
     * render the next frame.
     *
     * @return True if the window was not closed.
     */
    void run();

    virtual ~GraphicEngine();

    void onBodyCreation(const Body& newBody) override;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
};

#endif	/* GRAPHICENGINE_HPP */

