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

#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/shared_ptr.hpp"
#include "lua/types/LuaFunction.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/LuaStateView.hpp"
#include "SphereShape.hpp"
#include "StaticPlaneShape.hpp"

World::World(const Vector3<SI::Acceleration>& gravity) :
    broadPhase(std::make_unique<btDbvtBroadphase>()),
    collisionConfig(std::make_unique<btDefaultCollisionConfiguration>()),
    dispatcher(std::make_unique<btCollisionDispatcher>(collisionConfig.get())),
    solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
    world(std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadPhase.get(), solver.get(), collisionConfig.get()))
{
    world->setGravity(toBulletUnits(gravity));
}

void World::addObject(std::shared_ptr<Body> object) {
    Body& body = *object.get();
    world->addRigidBody(&body.getBulletBody());
    objects.insert(std::move(object));
    for (auto listener : createListener) {
        listener->onBodyCreation(body);
    }
}

void World::addConstraint(std::shared_ptr<btTypedConstraint> constraint) {
    world->addConstraint(constraint.get());
    constraints.insert(std::move(constraint));
}

Scalar<SI::Length> World::getDefaultMargin() {
    return fromBulletValue<SI::Length>(CONVEX_DISTANCE_MARGIN);
}

int World::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<World>;
    int result = 1;
    if (memberName == "gravity") {
        state.push<Vector3<SI::Acceleration>>(fromBulletValue<SI::Acceleration>(world->getGravity()));
    } else if (memberName=="newBody") {
        state.push<Method>([](World& object, LuaStateView& state) -> int {
            std::shared_ptr<Body> newBody = state.get<std::shared_ptr<Body>>(2);
            state.push<std::shared_ptr<Body>>(newBody);
            object.addObject(std::move(newBody));
            return 1;
        });
    } else if (memberName=="newShape") {
        state.push<LuaFunction>([](LuaStateView& state) -> int {
            std::shared_ptr<Shape> newShape = state.get<std::shared_ptr<Shape>>(1);
            state.push<std::shared_ptr<Shape>>(std::move(newShape));
            return 1;
        });
    } else if (memberName=="defaultMargin") {
        state.push<Scalar<SI::Length>>(getDefaultMargin());
    } else {
        result = 0;
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
        world->removeRigidBody(&object->getBulletBody());
    }
}
