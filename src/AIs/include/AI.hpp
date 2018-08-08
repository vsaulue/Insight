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

#ifndef AI_HPP
#define AI_HPP

#include <memory>

#include "AIInterface.hpp"
#include "lua/types/LuaTable.hpp"
#include "lua/types/LuaVirtualClass.hpp"

/** Abstract class for Artificial Intelligence implementations. */
class AI : public LuaVirtualClass {
public:
    /**
     * Creates a new AI.
     * @param interface Interface of the body controlled by this AI.
     */
    AI(AIInterface& interface) : interface(interface) {

    }

    virtual ~AI() = default;

    /**
     * Method called each time the AI should be updated.
     *
     * This method will be called by the simulation at regular time intervals,
     * when new values of the sense signals are available and this AI should update
     * the actions signals values.
     */
    virtual void stepSimulation() = 0;
protected:
    /** Interface (sense & action signals) of the body controlled by this AI. */
    AIInterface& interface;
};

#endif /* AI_HPP */
