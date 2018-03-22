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

#ifndef ACTIONINFO_HPP
#define ACTIONINFO_HPP

#include <unordered_set>

#include "Action.hpp"

/** Holds some information about a specific enum value. */
class ActionInfo {
private:
    /**
     * Creates a new record about a specific action value.
     *
     * @param[in] action Enum value described by this object.
     * @param[in] name Name of the action (associated string value).
     * @param[in] persistent Nature of the action (instant vs persistent).
     */
    ActionInfo(Action action, const char *name, bool persistent);
public:
    /**
     * Gets a set containing all ActionInfo objects.
     *
     * There will be 1 ActionInfo object for each possible value of Action.
     *
     * @return A set containing all ActionInfo records.
     */
    static const std::unordered_set<ActionInfo>& list();

    /**
     * Gets an ActionInfo record from the name of an action.
     *
     * @param[in] name Action name to look for.
     * @return nullptr if name is not valid, else the record associated to the action.
     */
    static const ActionInfo* byName(const std::string& name);

    bool operator==(const ActionInfo& other) const {
        return this->value == other.value;
    }

    /** Unique string representation of this enum value. */
    const std::string name;
    /** Enum value described by this ActionInfo object.*/
    const Action value;
    /** True if this is an action occuring over a period of time, false if it is instant. */
    const bool persistent;
};

namespace std {
    template<>
    struct hash<ActionInfo> {
        size_t operator()(const ActionInfo& actionInfo) const {
            return std::hash<Action>()(actionInfo.value);
        }
    };
}

#endif /* ACTIONINFO_HPP */
