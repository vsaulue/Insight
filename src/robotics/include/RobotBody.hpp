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

#include "Body.hpp"
#include "Joint.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "World.hpp"

/**
 * Object creating all robot parts and their constraints.
 *
 * Body parts are owned by the physics engine. Only references are kept inside
 * this object.
 */
class RobotBody : public LuaVirtualClass {
public:
    /**
     * Creates a new robot body.
     *
     * @param world World in which the body will be created.
     */
    RobotBody(World& world);

    virtual ~RobotBody();

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
private:
    /** Set of body parts, indexed by their names. */
    std::unordered_map<std::string, Body*> parts;
    /** Set of joints between body parts, indexed by their names. */
    std::unordered_map<std::string, std::unique_ptr<Joint>> joints;

    /**
     * Gets the reference body.
     *
     * The reference body is used to to compute new positions, velocities, ...
     *
     * @return The reference body.
     */
    Body& getBaseBody();
};

#endif /* ROBOTBODY_HPP */
