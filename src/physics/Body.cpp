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
#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/shared_ptr.hpp"
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

Body::Body(std::shared_ptr<Shape> shape) :
    shape(std::move(shape)),
    motionState(std::make_unique<MotionState>()),
    body(this->shape->getMass(), motionState.get(), &this->shape->getBulletShape(), this->shape->getInertia())
{

}

const btTransform& Body::getTransform() const {
    return body.getWorldTransform();
}


void Body::setPosition(const btVector3& newPos) {
    btTransform newTransform = getTransform();
    newTransform.setOrigin(newPos);
    setTransform(newTransform);
}

void Body::setRotation(const btQuaternion& newRotation) {
    btTransform newTransform = getTransform();
    newTransform.setRotation(newRotation);
    setTransform(newTransform);
}

void Body::setTransform(const btTransform& transform) {
    body.setWorldTransform(transform);
    body.setInterpolationWorldTransform(transform);
    motionState->setWorldTransform(transform);
}


btRigidBody& Body::getBulletBody() {
    return body;
}

int Body::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<Body>;
    int result = 1;
    if (memberName=="position") {
        state.push<btVector3>(getTransform().getOrigin());
    } else if (memberName=="setPosition") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            btVector3 newPos = state.get<btVector3>(2);
            object.setPosition(newPos);
            return 0;
        });
    } else if (memberName=="velocity") {
        state.push<btVector3>(body.getLinearVelocity());
    } else if (memberName=="setVelocity") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            btVector3 vel = state.get<btVector3>(2);
            object.body.setLinearVelocity(vel);
            return 0;
        });
    } else if (memberName=="rotation") {
        state.push<btQuaternion>(getTransform().getRotation());
    } else if (memberName=="setRotation") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            btQuaternion newRot = state.get<btQuaternion>(2);
            object.setRotation(newRot);
            return 0;
        });
    } else if (memberName=="shape") {
        state.push<std::shared_ptr<Shape>>(shape);
    } else {
        result = 0;
    }
    return result;
}

std::unique_ptr<Body> Body::luaGetFromTable(LuaTable& table) {
    std::shared_ptr<Shape> shape = table.get<LuaNativeString,std::shared_ptr<Shape>>("shape");
    // TODO: position, orientation, velocity, angular velocity...
    return std::make_unique<Body>(shape);
}

void Body::addMoveListener(BodyMoveListener& listener) const {
    motionState->addListener(listener);
}

void Body::removeMoveListener(BodyMoveListener& listener) const {
    motionState->removeListener(listener);
}

Body::~Body() = default;
