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

#ifndef SHELLINTERPRETERCONFIG_HPP
#define SHELLINTERPRETERCONFIG_HPP



#include "lua/LuaStateView.hpp"

/** Enables ShellInterpreter users to set some custom callbacks. */
class ShellInterpreterConfig {
public:
    /**
     * Called once when the shell is initialised.
     *
     * @param state Newly created Lua state.
     */
    virtual void init(LuaStateView& state) = 0;

    /**
     * Called once before executing a new command from stdin.
     *
     * @param state The Lua state of the shell.
     */
    virtual void beforeCommand(LuaStateView& state) = 0;

    /**
     * Called once after having executed a command from stdin.
     *
     * @param state The Lua state of the shell.
     */
    virtual void afterCommand(LuaStateView& state) = 0;

    virtual ~ShellInterpreterConfig() = default;
};

#endif /* SHELLINTERPRETERCONFIG_HPP */
