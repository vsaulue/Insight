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

#ifndef ROBOT_HPP
#define ROBOT_HPP

#include <memory>

#include "AI.hpp"
#include "AIFactory.hpp"
#include "RobotBody.hpp"

/** Structure owning a Robot body & its AI. */
struct Robot {
    Robot(World& world, std::shared_ptr<RobotBody::ConstructionInfo> bodyInfo, const AIFactory& aiFactory) :
        body(std::make_unique<RobotBody>(world, bodyInfo)),
        ai(aiFactory.createAI(body->getInterface()))
    {

    }

    /** Body of this robot. */
    std::unique_ptr<RobotBody> body;
    /** AI of this robot. */
    std::unique_ptr<AI> ai;
};

#endif /* ROBOT_HPP */
