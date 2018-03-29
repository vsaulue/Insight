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

#ifndef WORLD_HPP
#define WORLD_HPP

#include <memory>
#include <unordered_set>

#include "btBulletDynamicsCommon.h"

#include "Body.hpp"
#include "BodyCreationListener.hpp"
#include "lua/types/LuaVirtualClass.hpp"

/**
 * Physics engine
 *
 * Holds all objects (Body) with their properties, and integrate mechanics laws.
 */
class World : public LuaVirtualClass {
public:
    /** Constant iterator over all the bodies of this engine. */
    using const_iterator = std::unordered_set<std::unique_ptr<Body>>::const_iterator;

    /** Creates a new empty world with default settings.*/
    World();

    /** Iterates over all the objects in this World. */
    const_iterator begin() const {
        return objects.begin();
    }

    /** Iterates over all the objects in this World. */
    const_iterator end() const {
        return objects.end();
    }

    /**
     * Adds a new object into the world.
     *
     * @param object The new object.
     */
    void addObject(std::unique_ptr<Body>&& object);

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    /**
     * Runs a new step of the simulation.
     *
     * @param[in] timeStep Duration of the step.
     */
    void stepSimulation(double timeStep) {
        world->stepSimulation(timeStep);
    }

    /**
     * Adds a new listener for "new Body" events.
     *
     * @param listener The new listener.
     */
    void addCreationListener(BodyCreationListener& listener) const;

    /**
     * Removes a listener for 'new Body" events.
     * @param listener The listener to remove.
     */
    void removeCreationListener(BodyCreationListener& listener) const;

    virtual ~World();
private:
    /** Broad phase algorithm for collision detection. */
    std::unique_ptr<btBroadphaseInterface> broadPhase;
    /** Narrow phase collision detction configuration. */
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfig;
    /** Configuration of narrow phase collision detection. */
    std::unique_ptr<btDispatcher> dispatcher;
    /** Integrator (use forces, constraints to compute speed & positions). */
    std::unique_ptr<btConstraintSolver> solver;
    /** Bullet world. */
    std::unique_ptr<btDiscreteDynamicsWorld> world;

    /** List of objects in the world. */
    std::unordered_set<std::unique_ptr<Body>> objects;
    /** List of objects to inform of new Bodies. */
    mutable std::unordered_set<BodyCreationListener*> createListener;
};

#endif /* WORLD_HPP */
