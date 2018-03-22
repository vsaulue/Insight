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

#include <functional>
#include <map>
#include <memory>

#include "InputEvent.hpp"

/** Creates InputEvents from a given name (std::string).*/
class InputEventFactory {
public:
    /**
     * Function used to creates new events from any input device.
     *
     * <li>
     * <ul>param persistent (bool): see makeByName().
     * <ul>returns An event</ul>
     * </li>
     */
    using EventMaker = std::function<std::unique_ptr<InputEvent>(bool)>;

    /**
     * Gets the map of all event names, mapped to function creating these events.
     *
     * @return A map of EventMaker, indexed by the event names.
     */
    static const std::map<std::string, EventMaker>& list();

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
