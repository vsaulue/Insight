/* This file is part of Insight.
 * Copyright (C) 2017 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#include "LuaException.hpp"
#include "LuaState.hpp"
#include "LuaVirtualClass.hpp"

#include "ShellInterpreter.hpp"

#include <iostream>

ShellInterpreter::ShellInterpreter(LuaVirtualClass& rootObject, const std::string& rootName) : rootObject(rootObject), rootName(rootName) {
    running = false;
}

void ShellInterpreter::run() {
    LuaState luaState;
    running = true;
    std::string line;

    luaState.open_base();

    luaState.newObject<LuaVirtualClass*>(&rootObject); // stack index 1

    rootObject.luaPushMetatable(luaState);
    luaState.setMetatable(-2);

    luaState.setGlobal(rootName);

    while (running && std::getline(std::cin, line)) {
        try {
            luaState.doString(line);
        } catch (const LuaException &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

void ShellInterpreter::quit() {
    running = false;
}


LuaVirtualClass& ShellInterpreter::getRootObject() {
    return rootObject;
}

const std::string& ShellInterpreter::luaClassName() const {
    static const std::string className("ShellInterpreter");
    return className;
}

void ShellInterpreter::luaPopulateIndex(LuaStateView& luaState) {
    luaState.pop(1);
}
