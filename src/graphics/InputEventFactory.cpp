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

#include "KeyInputEvent.hpp"
#include "InputEventFactory.hpp"
#include "MouseMoveEvent.hpp"

/**
 * Initialises the return value of InputEventFactory::list().
 *
 * @return A set of all input names.
 */
static std::set<std::string> listInitializer() {
    std::set<std::string> result;
    for (const auto& pair : KeyInputEvent::keyNameMap()) {
        result.insert(pair.second);
    }
    for (const auto& pair : MouseMoveEvent::directionNameMap()) {
        result.insert(pair.second);
    }
    return result;
}

const std::set<std::string>& InputEventFactory::list() {
    static const std::set<std::string> result = listInitializer();
    return result;
}

std::unique_ptr<InputEvent> InputEventFactory::makeByName(const std::string& name, bool persistent) {
    std::unique_ptr<InputEvent> result(nullptr);
    if (name.find("KEY_") == 0) {
        result = KeyInputEvent::makeByName(name, persistent);
    } else if (name.find("MOUSE_Move") == 0) {
        result = MouseMoveEvent::makeByName(name, persistent);
    }
    return result;
}
