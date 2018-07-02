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

World::World() :
    broadPhase(std::make_unique<btDbvtBroadphase>()),
    collisionConfig(std::make_unique<btDefaultCollisionConfiguration>()),
    dispatcher(std::make_unique<btCollisionDispatcher>(collisionConfig.get())),
    solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
    world(std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadPhase.get(), solver.get(), collisionConfig.get())),
    worldUpdater(*world)
{
    static const Vector3<SI::Acceleration> DEFAULT_GRAVITY(0, -9.8, 0);
    world->setGravity(toBulletUnits(DEFAULT_GRAVITY));
}

Vector3<SI::Acceleration> World::getGravity() const {
    return fromBulletValue<SI::Acceleration>(world->getGravity());
}

void World::setGravity(const Vector3<SI::Acceleration>& value) {
    world->setGravity(toBulletUnits(value));
}

void World::addObject(std::shared_ptr<Body> object) {
    Body& body = *object.get();
    world->addRigidBody(&body.getBulletBody());
    object->setWorldUpdater(&worldUpdater);
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
        state.push<Vector3<SI::Acceleration>>(getGravity());
    } else if (memberName == "setGravity") {
        state.push<Method>([](World& object, LuaStateView& state) -> int {
            auto value = state.get<Vector3<SI::Acceleration>>(2);
            object.setGravity(value);
            return 0;
        });
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

void World::stepSimulation(double timeStep) {
    worldUpdater.newFrame();
    world->stepSimulation(timeStep,4,btScalar(1/240.0));
}

void World::addCreationListener(BodyCreationListener& listener) const {
    createListener.insert(&listener);
}

void World::removeCreationListener(BodyCreationListener& listener) const {
    createListener.erase(&listener);
}


World::~World() {
    for (auto& object : objects) {
        object->setWorldUpdater(nullptr);
        world->removeRigidBody(&object->getBulletBody());
    }
}
