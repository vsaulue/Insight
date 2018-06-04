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

#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/LuaException.hpp"
#include "lua/LuaState.hpp"

#include "ShellInterpreter.hpp"

#include <iostream>

ShellInterpreter::ShellInterpreter(ShellInterpreterConfig& config) : config(config) {
    running = false;
}

void ShellInterpreter::run() {
    LuaState luaState;
    running = true;

    config.init(luaState);

    std::string line;
    while (running && std::getline(std::cin, line)) {
        config.beforeCommand(luaState);
        try {
            luaState.doString(line);
        } catch (const LuaException &e) {
            std::cerr << e.what() << std::endl;
        }
        config.afterCommand(luaState);
    }
}

void ShellInterpreter::quit() {
    running = false;
}

int ShellInterpreter::luaIndex(const std::string& memberName, LuaStateView& state) {
    return 0;
}
