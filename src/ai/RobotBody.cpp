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

#include "RobotBody.hpp"

/** Average density of body parts (kg/m^3).*/
static const btScalar DENSITY = 1500.0f;
/** Head radius (m). */
static const btScalar HEAD_RADIUS = 0.2f;

static btScalar getSphereMass(btScalar radius, btScalar density) {
    return density*4/3*SIMD_PI*radius*radius*radius;
}

static Sphere& createHead(World& world) {
    std::unique_ptr<Sphere> head = std::make_unique<Sphere>(getSphereMass(HEAD_RADIUS, DENSITY), HEAD_RADIUS);
    Sphere& result = *head.get();
    world.addObject(std::move(head));
    return result;
}

RobotBody::RobotBody(World& world) :
    head(createHead(world))
{
    head.setPosition(btVector3(0,2,0));
}
