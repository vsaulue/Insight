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

#ifndef ROBOTBODY_HPP
#define ROBOTBODY_HPP

#include <string>
#include <unordered_map>

#include "CompoundBody.hpp"
#include "Sphere.hpp"
#include "SphericalJoint.hpp"
#include "World.hpp"

/**
 * Object creating all robot parts and their constraints.
 *
 * Body parts are owned by the physics engine. Only references are kept inside
 * this object.
 */
class RobotBody {
public:
    /**
     * Creates a new robot body.
     *
     * @param world World in which the body will be created.
     */
    RobotBody(World& world);
private:
    std::unordered_map<std::string, CompoundBody*> parts;

    std::unordered_map<std::string, std::unique_ptr<SphericalJoint>> joints;

    CompoundBody& insert(World& world, const std::string& name, std::unique_ptr<CompoundBody> part);
};

#endif /* ROBOTBODY_HPP */
