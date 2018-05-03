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

#include <unordered_set>

#include "Body.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/types/LuaMethod.hpp"

/** Custom implementation of btMotionState. */
class Body::MotionState : public btMotionState {
private:
    /** List of listeners to inform of new object position & orientation. */
    std::unordered_set<BodyMoveListener*> moveListener;
public:
    void getWorldTransform(btTransform& worldTrans) const override {
        worldTrans.setIdentity();
    }

    void setWorldTransform(const btTransform& worldTrans) override {
        for (auto listener: moveListener) {
            listener->onBodyMove(worldTrans);
        }
    }

    /**
     * Adds a new listener for "move" events.
     *
     * @param listener The new listener.
     */
    void addListener(BodyMoveListener& listener) {
        moveListener.insert(&listener);
    }

    /**
     * Removes listener for "move" events.
     *
     * @param listener The listener to remove.
     */
    void removeListener(BodyMoveListener& listener) {
        moveListener.erase(&listener);
    }
};

/**
 * Recalculate the moments of inertia of a given shape.
 * @param shape Collision shape of the object.
 * @param mass Mass of the object.
 * @return A vector containing the moment of inertia around each axis.
 */
static btVector3 calculateInertia(const btCollisionShape& shape, btScalar mass) {
    btVector3 result(0,0,0);
    if (mass != 0) {
        shape.calculateLocalInertia(mass, result);
    }
    return result;
}

Body::Body(btScalar mass, btCollisionShape& shape) :
    motionState(std::make_unique<MotionState>()),
    btBody(std::make_unique<btRigidBody>(mass, motionState.get(), &shape, calculateInertia(shape, mass)))
{

}

const btTransform& Body::getTransform() const {
    return btBody->getWorldTransform();
}


void Body::setPosition(const btVector3& newPos) {
    btTransform newTransform = btBody->getWorldTransform();
    newTransform.setOrigin(newPos);
    btBody->setWorldTransform(newTransform);
    btBody->setInterpolationWorldTransform(newTransform);
    motionState->setWorldTransform(newTransform);
}

void Body::setTransform(const btTransform& transform) {
    btBody->setWorldTransform(transform);
    btBody->setInterpolationWorldTransform(transform);
    motionState->setWorldTransform(transform);
}


btRigidBody* Body::getBulletBody() {
    return btBody.get();
}

static int luaReturnVector(const btVector3& vector, LuaStateView& state) {
    state.push<double>(vector.getX());
    state.push<double>(vector.getY());
    state.push<double>(vector.getZ());
    return 3;
}

static btVector3 luaGetVector(LuaStateView& state, int firstStackIndex) {
    double x = state.get<double>(firstStackIndex);
    double y = state.get<double>(firstStackIndex+1);
    double z = state.get<double>(firstStackIndex+2);
    return btVector3(x,y,z);
}

int Body::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<Body>;
    int result = 0;
    if (memberName=="position") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            const btVector3& pos = object.btBody->getWorldTransform().getOrigin();
            return luaReturnVector(pos, state);
        });
        result = 1;
    } else if (memberName=="setPosition") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            btVector3 newPos = luaGetVector(state, 2);
            object.setPosition(newPos);
            return 0;
        });
        result = 1;
    } else if (memberName=="velocity") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            const btVector3& vel = object.btBody->getLinearVelocity();
            return luaReturnVector(vel, state);
        });
        result = 1;
    } else if (memberName=="setVelocity") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            btVector3 vel = luaGetVector(state, 2);
            object.btBody->setLinearVelocity(vel);
            return 0;
        });
        result = 1;
    }
    return result;
}

void Body::addMoveListener(BodyMoveListener& listener) const {
    motionState->addListener(listener);
}

void Body::removeMoveListener(BodyMoveListener& listener) const {
    motionState->removeListener(listener);
}

Body::~Body() = default;

void Body::recalculateInertia(btScalar newMass) {
    btBody->setMassProps(newMass, calculateInertia(*btBody->getCollisionShape(), newMass));
}
