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
#include "units/BulletUnits.hpp"

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
    body(toBulletUnits(this->shape->getMass()), motionState.get(), &this->shape->getBulletShape(), this->shape->getEngineInertia()),
    worldUpdater(nullptr)
{
    setSleepingThresholds(Scalar<SI::Speed>(0.01), Scalar<SI::AngularVelocity>(SIMD_PI / 10.0));
}

const btTransform& Body::getEngineTransform() const {
    return body.getWorldTransform();
}

Vector3<SI::Length> Body::getPosition() const {
    return fromBulletValue<SI::Length>(body.getWorldTransform().getOrigin());
}

void Body::setPosition(const Vector3<SI::Length>& newPos) {
    btTransform newTransform = getEngineTransform();
    newTransform.setOrigin(toBulletUnits(newPos));
    setEngineTransform(newTransform);
}

void Body::setRotation(const btQuaternion& newRotation) {
    btTransform newTransform = getEngineTransform();
    newTransform.setRotation(newRotation);
    setEngineTransform(newTransform);
}

void Body::setEngineTransform(const btTransform& transform) {
    if (worldUpdater != nullptr) {
        worldUpdater->updateTransform(body, transform);
    } else {
        body.setWorldTransform(transform);
        body.setInterpolationWorldTransform(transform);
    }
    motionState->setWorldTransform(transform);
}

Vector3<SI::Speed> Body::getLinearVelocity() const {
    return fromBulletValue<SI::Speed>(body.getLinearVelocity());
}

Vector3<SI::AngularVelocity> Body::getAngularVelocity() const {
    return fromBulletValue<SI::AngularVelocity>(body.getAngularVelocity());
}

void Body::applyAngularImpulse(const Vector3<SI::AngularVelocity>& delta) {
    btVector3 bulletDelta = toBulletUnits(delta);
    body.setAngularVelocity(bulletDelta + body.getAngularVelocity());
}

Vector3<SI::InvAngularMass> Body::getInvInertiaDiagLocal() const {
    return fromBulletValue<SI::InvAngularMass>(body.getInvInertiaDiagLocal());
}

void Body::setLinearVelocity(const Vector3<SI::Speed>& velocity) {
    btVector3 engineVelocity = toBulletUnits(velocity);
    body.setLinearVelocity(engineVelocity);
    body.setInterpolationLinearVelocity(engineVelocity);
    body.activate();
}

btRigidBody& Body::getBulletBody() {
    return body;
}

void Body::setWorldUpdater(WorldUpdater* newValue) {
    worldUpdater = newValue;
}

int Body::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<Body>;
    int result = 1;
    if (memberName=="position") {
        state.push<Vector3<SI::Length>>(getPosition());
    } else if (memberName=="setPosition") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            auto newPos = state.get<Vector3<SI::Length>>(2);
            object.setPosition(newPos);
            return 0;
        });
    } else if (memberName=="velocity") {
        state.push<Vector3<SI::Speed>>(getLinearVelocity());
    } else if (memberName=="setVelocity") {
        state.push<Method>([](Body& object, LuaStateView& state) -> int {
            auto vel = state.get<Vector3<SI::Speed>>(2);
            object.setLinearVelocity(vel);
            return 0;
        });
    } else if (memberName=="rotation") {
        state.push<btQuaternion>(getEngineTransform().getRotation());
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

void Body::setSleepingThresholds(Scalar<SI::Speed> linear, Scalar<SI::AngularVelocity> angular) {
    body.setSleepingThresholds(toBulletUnits(linear), toBulletUnits(angular));
}
