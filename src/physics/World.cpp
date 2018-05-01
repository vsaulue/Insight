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

#include "World.hpp"

#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/LuaStateView.hpp"
#include "Sphere.hpp"
#include "Terrain.hpp"

World::World() :
    broadPhase(std::make_unique<btDbvtBroadphase>()),
    collisionConfig(std::make_unique<btDefaultCollisionConfiguration>()),
    dispatcher(std::make_unique<btCollisionDispatcher>(collisionConfig.get())),
    solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
    world(std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadPhase.get(), solver.get(), collisionConfig.get()))
{

}

void World::addObject(std::unique_ptr<Body>&& object) {
    Body& body = *object.get();
    world->addRigidBody(object->getBulletBody());
    objects.insert(std::move(object));
    for (auto listener : createListener) {
        listener->onBodyCreation(body);
    }
}

void World::addConstraint(std::shared_ptr<btTypedConstraint> constraint) {
    world->addConstraint(constraint.get());
    constraints.insert(std::move(constraint));
}

int World::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<World>;
    int result = 0;
    if (memberName=="newSphere") {
        state.push<Method>([](World& object, LuaStateView& state) -> int {
            std::unique_ptr<Sphere> newObject = std::make_unique<Sphere>(1,1);
            state.push<Body*>(newObject.get());
            object.addObject(std::move(newObject));
            return 1;
        });
        result = 1;
    } else if (memberName=="newTerrain") {
        state.push<Method>([](World& object, LuaStateView& state) -> int {
            std::unique_ptr<Terrain> newObject = std::make_unique<Terrain>();
            state.push<Body*>(newObject.get());
            object.addObject(std::move(newObject));
            return 1;
        });
        result = 1;
    }
    return result;
}

void World::addCreationListener(BodyCreationListener& listener) const {
    createListener.insert(&listener);
}

void World::removeCreationListener(BodyCreationListener& listener) const {
    createListener.erase(&listener);
}


World::~World() {
    for (auto& object : objects) {
        world->removeRigidBody(object->getBulletBody());
    }
}
