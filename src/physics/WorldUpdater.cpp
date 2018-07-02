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

#include "WorldUpdater.hpp"

WorldUpdater::WorldUpdater(btDiscreteDynamicsWorld& world) :
    world(world),
    allReactivated(false)
{

}

void WorldUpdater::newFrame() {
    allReactivated = false;
}

void WorldUpdater::updateTransform(btRigidBody& body, const btTransform& newTransform) {
    // Note: a clever & efficient way to update only the adjacent bodies would be nice.
    if (!allReactivated) {
        auto& objects = world.getCollisionObjectArray();
        for (int index = 0; index < objects.size(); index++) {
            objects[index]->activate();
        }
        allReactivated = true;
    }
    world.removeRigidBody(&body);
    body.setWorldTransform(newTransform);
    body.setInterpolationWorldTransform(newTransform);
    world.addRigidBody(&body);
}
