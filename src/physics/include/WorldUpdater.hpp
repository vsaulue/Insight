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

#ifndef WORLDUPDATER_HPP
#define WORLDUPDATER_HPP

#include "btBulletDynamicsCommon.h"

/** Interface used by bodies inside a world to update their attributes. */
class WorldUpdater {
public:
    /**
     * Creates a new world updater.
     * @param world Bullet object implementing the world.
     */
    WorldUpdater(btDiscreteDynamicsWorld& world);

    /**
     * Signal to this object that a new frame will be rendered.
     */
    void newFrame();

    /**
     * Callback used by a Body to update its transform.
     * @param body Bullet body that will be moved.
     * @param newTransform The new transform of the specified body.
     */
    void updateTransform(btRigidBody& body, const btTransform& newTransform);
private:
    /** Bullet world updated by this object. */
    btDiscreteDynamicsWorld& world;
    /** Flag set when all objects in the world have been reactivated. */
    bool allReactivated;
};

#endif /* WORLDUPDATER_HPP */
