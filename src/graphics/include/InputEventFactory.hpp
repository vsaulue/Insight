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

#ifndef INPUTEVENTFACTORY_HPP
#define INPUTEVENTFACTORY_HPP

#include <set>
#include <memory>

#include "InputEvent.hpp"

/** Creates InputEvents from a given name (std::string).*/
class InputEventFactory {
public:
    /**
     * Gets a set of all events names.
     *
     * A event name is a unique string designing an elementary input: a keyboard key,
     * a mouse button, a mouse movement in a direction, ...
     */
    static const std::set<std::string>& list();

    /**
     * Creates an InputEvent from an event name.
     *
     * @param[in] name Event name.
     * @param[in] persistent true for events that occurs over a certain period of time; false for
     * instant events. It is used to generate appropriate input events. For example, persistent
     * keyboard events are triggered every frame when the key is being held down, instant events are
     * triggered only when the key state changes.
     *
     * @return A null pointer if the event name is invalid, else an event
     */
    static std::unique_ptr<InputEvent> makeByName(const std::string& name, bool persistent);
};

#endif /* INPUTEVENTFACTORY_HPP */
