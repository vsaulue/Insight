/* This file is part of Insight.
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

#ifndef LUAEXCEPTION_HPP
#define LUAEXCEPTION_HPP

#include <stdexcept>

struct lua_State;

/**
 * Exception raised when some Lua code has an invalid syntax.
 */
class LuaException : public std::exception {
private:
    /** Underlying C state (used to retrieve an error message). */
    lua_State* state;
public:
    /**
     * Creates a new LuaException.
     *
     * @param state State that raised the Lua error.
     */
    LuaException(lua_State* state);

    virtual const char* what() const noexcept override;

    virtual ~LuaException();
};

#endif /* LUAEXCEPTION_HPP */

