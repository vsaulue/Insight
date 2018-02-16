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
#include "lua/types/LuaVirtualClass.hpp"

class World : public LuaVirtualClass {
public:
    using const_iterator = std::unordered_set<std::unique_ptr<Body>>::const_iterator;

    World();

    const_iterator begin() const {
        return objects.begin();
    }

    const_iterator end() const {
        return objects.end();
    }

    void addObject(std::unique_ptr<Body>&& object);

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

private:
    std::unique_ptr<btBroadphaseInterface> broadPhase;
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfig;
    std::unique_ptr<btDispatcher> dispatcher;
    std::unique_ptr<btConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> world;

    std::unordered_set<std::unique_ptr<Body>> objects;
};

#endif /* WORLD_HPP */
