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

#ifndef FEEDBACKLOOP_HPP
#define FEEDBACKLOOP_HPP

#include "lua/types/LuaVirtualClass.hpp"

/** Single feedback loop, controlling a specific joint. */
class FeedbackLoop : public LuaVirtualClass {
public:
    virtual ~FeedbackLoop() = default;

    /**
     * Updates output (motor) values from current state & input (position) values.
     *
     * This method is called at regular time interval by the simulation.
     */
    virtual void stepSimulation() = 0;
};

#endif /* FEEDBACKLOOP_HPP */
