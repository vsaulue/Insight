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

#ifndef GRAPHICOBJECT_HPP
#define GRAPHICOBJECT_HPP

#include <memory>

#include "irrlicht.h"

#include "Body.hpp"

/**
 * Representation of an object from the physics engine.
 */
class GraphicObject : public BodyMoveListener {
public:
    /**
     * Creates a 3d node representing an object from the physics engine.
     *
     * @param body Object of the simulation to render.
     * @param scene Scene in which this object should be rendered.
     */
    GraphicObject(const Body& body, irr::scene::ISceneManager& scene);

    void onBodyMove(const btTransform& transform) override;

    virtual ~GraphicObject();
private:
    /** Object in the physics engine represented by this GraphicObject. */
    const Body& body;

    /**
     * Custom deleter for a scene node (for std::unique_ptr).
     */
    struct NodeDeleter {
    public:
        void operator()(irr::scene::ISceneNode* object) {
            object->remove();
        }
    };

    /** Irrlicht node containing the 3d objects of this GraphicObject.*/
    std::unique_ptr<irr::scene::ISceneNode, NodeDeleter> node;

    /**
     * Updates the position & location of this object.
     *
     * @param[in] transform New position & orientation of the object.
     */
    void updateTransform(const btTransform& transform);
};

#endif /* GRAPHICOBJECT_HPP */

