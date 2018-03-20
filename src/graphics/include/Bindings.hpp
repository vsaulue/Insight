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

#ifndef BINDINGS_HPP
#define BINDINGS_HPP

#include <memory>
#include <unordered_map>

#include "irrlicht.h"

#include "Action.hpp"
#include "InputEvent.hpp"
#include "lua/types/LuaVirtualClass.hpp"

/** Maps user actions in the GUI to mouse & keyboard buttons. */
class Bindings : public LuaVirtualClass {
public:
    /**
     * Gets the event binded to the given action.
     * @param[in] action Action to check.
     * @return The event used to trigger this action.
     */
    InputEvent& getEvent(Action action) const {
        return *actionToKey.at(action);
    }

    /** Constructs a new bindings with a default mapping for QWERTY keyboards. */
    Bindings();

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

private:
    std::unordered_map<Action, std::unique_ptr<InputEvent>> actionToKey;
};

#endif /* BINDINGS_HPP */

