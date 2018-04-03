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

Body::Body(btScalar mass, btCollisionShape& shape, const btVector3& inertia) :
    motionState(std::make_unique<MotionState>()),
    btBody(std::make_unique<btRigidBody>(mass, motionState.get(), &shape, inertia))
{

}

const btVector3& Body::getPosition() const {
    return btBody->getWorldTransform().getOrigin();
}

const btMatrix3x3& Body::getRotation() const {
    return btBody->getWorldTransform().getBasis();
}


void Body::setPosition(const btVector3& newPos) {
    btTransform newTransform = btBody->getWorldTransform();
    newTransform.setOrigin(newPos);
    btBody->setWorldTransform(newTransform);
    btBody->setInterpolationWorldTransform(newTransform);
    motionState->setWorldTransform(newTransform);
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
