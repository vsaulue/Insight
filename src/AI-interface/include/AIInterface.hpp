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

#ifndef AIINTERFACE_HPP
#define AIINTERFACE_HPP

#include <unordered_map>

#include "ActionSignal.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "SenseSignal.hpp"

/** Interface used by AIs to interact with the world. */
class AIInterface : public LuaVirtualClass {
public:
    /** Creates a new empty interface (no input or output signals). */
    AIInterface();

    virtual ~AIInterface();

    /**
     * Get the set of output signals of the AI, indexed by their names.
     * @return The map of action signals, indexed by name.
     */
    std::unordered_map<std::string,ActionSignal*>& getActions() {
        return actions;
    }

    /**
     * Get the set of input signals of the AI, indexed by their names.
     * @return The map of sense signals, indexed by name.
     */
    std::unordered_map<std::string,SenseSignal*>& getSenses() {
        return senses;
    }

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
private:
    /** Map: action name to signal. */
    std::unordered_map<std::string,ActionSignal*> actions;
    /** Map: sense name to signal. */
    std::unordered_map<std::string,SenseSignal*> senses;
};

#endif /* AIINTERFACE_HPP */

