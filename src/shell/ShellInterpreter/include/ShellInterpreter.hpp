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

#ifndef SHELLINTERPRETER_HPP
#define	SHELLINTERPRETER_HPP

#include <string>
#include <vector>

#include "lua/types/LuaVirtualClass.hpp"

/**
 * Shell implementation.
 *
 * Reads statements from standard input, evaluates them and prints the result
 * on standard output.
 */
class ShellInterpreter : public LuaVirtualClass {
public:
    /**
     * Creates a new shell.
     *
     * @param[in,out] rootObject Main object of the shell.
     * @param[in] rootName Name of the variable in the shell that will contain rootObject.
     */
    ShellInterpreter(LuaVirtualClass& rootObject, const std::string& rootName);

    /**
     * Run the shell.
     *
     * It contains the main loop reading from standard input.
     *
     * @param[in] luaInitScripts List of script to execute before reading from stdin.
     */
    void run(const std::vector<std::string>& luaInitScripts);

    /**
     * Set a flag making the shell to stop after evaluating the current statement.
     */
    void quit();

    /**
     * Get the root object of this shell.
     *
     * @return The root object of this shell.
     */
    LuaVirtualClass& getRootObject();

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

private:
    /** Root object of this shell.*/
    LuaVirtualClass& rootObject;

    /** Name of the global variable containing rootObject. */
    std::string rootName;

    /** Flag telling if the main loop should keep running. */
    bool running;
};

#endif	/* SHELLINTERPRETER_HPP */

