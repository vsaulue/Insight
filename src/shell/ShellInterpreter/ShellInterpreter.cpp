/* This file is part of Insight.
 * Copyright (C) 2017-2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/LuaException.hpp"
#include "lua/LuaState.hpp"

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

    luaState.push<LuaVirtualClass*>(&rootObject); // stack index 1

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

int ShellInterpreter::luaIndex(const std::string& memberName, LuaStateView& state) {
    return 0;
}
