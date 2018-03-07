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

#include <unordered_map>

#include "irrlicht.h"

#include "Action.hpp"

/** Maps user actions in the GUI to mouse & keyboard buttons. */
class Bindings {
public:
    /**
     * Gets the key binded to the given action.
     * @param[in] action Action to check.
     * @return The key used to trigger an action.
     */
    irr::EKEY_CODE getKey(Action action) const {
        return actionToKey.at(action);
    }

    /**
     * Gets the action binded to the given key, if there is one.
     * @param key
     * @param action
     * @return True if there is an action binded to key.
     */
    bool getAction(irr::EKEY_CODE key, Action& action) const {
        const auto it = keyToAction.find(key);
        bool found = (it != keyToAction.end());
        if (found) {
            action = it->second;
        }
        return found;
    }

    /** Constructs a new bindings with a default mapping for QWERTY keyboards. */
    Bindings();
private:
    /** Map giving the key associated to an action.*/
    std::unordered_map<Action, irr::EKEY_CODE> actionToKey;
    /** Map giving the action associated to a key. */
    std::unordered_map<irr::EKEY_CODE, Action> keyToAction;

    /** Builds keyToAction from actionToKey. */
    void buildKeyToAction();
};

#endif /* BINDINGS_HPP */

