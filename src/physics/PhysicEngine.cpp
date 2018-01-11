/* This file is part of Insight.
 * Copyright (C) 2017-2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#include "PhysicEngine.hpp"

Universe& PhysicEngine::getUniverse() {
    return universe;
}

const Universe& PhysicEngine::getUniverse() const {
    return universe;
}

void PhysicEngine::integrate() {
    for (auto it = universe.begin(); it != universe.end(); ++it) {
        PhysicalObject& obj(**it);
        Distance (&p)[3](obj.getPosition());
        Speed (&v)[3](obj.getVelocity());
        for (int i = 0; i < 3; i++) {
            p[i] = p[i] + Distance(v[i]);
        }
    }
}

const std::string& PhysicEngine::luaClassName() {
    static const std::string className("PhysicEngine");
    return className;
}

void PhysicEngine::luaPopulateIndex(LuaStateView& luaState) {
    // TODO: implement methods.
    luaState.pop(1);
}