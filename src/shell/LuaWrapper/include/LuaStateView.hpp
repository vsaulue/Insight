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

#ifndef LUASTATEVIEW_HPP
#define LUASTATEVIEW_HPP

#include <string>

struct lua_State;
struct luaL_Reg;

/**
 * Wraps an existing C lua_State.
 */
class LuaStateView {
protected:
    /** lua_State from the C API. */
    lua_State* state;
public:

    /**
     * Creates a new interpreter.
     */
    LuaStateView(lua_State* state);

    /**
     * Loads and executes the given string as Lua code.
     *
     * @param[in] code Lua code to execute.
     * @throws LuaException If the Lua code could not be evaluated.
     */
    void doString(const std::string& code);

    virtual ~LuaStateView();

    /**
     * Creates a new Lua userdata of the specified size.
     *
     * @param[in] size Size of the memory block.
     *
     * @return A pointer to the new memory block.
     */
    void* newUserData(std::size_t size);

    /**
     * Creates a new Lua userdata wrapping a new C++ object.
     *
     * @tparam UserDataType Type of the created C++ object.
     * @tparam ArgsType Types of the constructor arguments of UserDataType.
     *
     * @param constructorArgs Arguments passed to the constructor of the new C++ object.
     * @return A pointer to the new C++ object.
     */
    template<typename UserDataType, typename... ArgsType>
    UserDataType* newObject(ArgsType&&... constructorArgs) {
        void *mem = newUserData(sizeof(UserDataType));
        UserDataType* result = new(mem) UserDataType(std::forward<ArgsType>(constructorArgs)...);
        // TODO: set c++ destructor in LUA metatable.
        return result;
    }

    /**
     * Pops a value from the stack and sets it as the new value of global name.
     *
     * @param[in] name Name of the global variable.
     */
    void setGlobal(const std::string& name);

    /**
     * Pushes a copy of the element at the given valid index onto the stack.
     *
     * @param[in] stackIndex Index of the element to copy.
     */
    void pushValue(int stackIndex);

    /**
     * Get (or create) a metatable in the Lua registry.
     *
     * @param[in] name Name of the metatable in the registry.
     * @return True in a new table was create. False if the table "name" already existed.
     */
    bool newMetatable(const std::string& name);

    /**
     * Pops a metatable from the stack and sets it to the value at the given acceptable index.
     *
     * @param[in] stackIndex Index of the object receiving the metatable.
     */
    void setMetatable(int stackIndex);

    /**
     * Pushes a copy of the given string onto the Lua stack.
     *
     * @param[in] str Value to be pushed on the stack.
     */
    void pushString(const std::string& str);

    /**
     * Set a value in a Lua table.
     *
     * Does the equivalent to t[k]=v in Lua, where:
     * - t is the value in the Lua stack at stackIndex.
     * - v is the first value popped from the top of the stack.
     * - k is the second value popped from the top of the stack.
     *
     * @param[in] stackIndex Index of the table being modified.
     */
    void setTable(int stackIndex);

    /**
     * Creates a new empty table and pushes it onto the stack.
     */
    void newTable();

    /**
     * Sets a value in a Lua table.
     *
     * Does the equivalent of t[fieldName]=v in Lua, where:
     * - t is the value in the Lua stack at stackIndex.
     * - v is the value popped from the top of the stack.
     *
     * @param[in] stackIndex Index of the table being modified.
     * @param[in] fieldName Key of the modified value in the table.
     */
    void setField(int stackIndex, const std::string &fieldName);

    /**
     * Registers C++ functions into the table on the top of the stack.
     *
     * @param[in] functions Array of function names & function pointers.
     * @param[in] nup Number of Upvalues popped from the Lua stack.
     */
    void setFuncs(const luaL_Reg *functions, int nup);

    /**
     * Checks whether the object in the Lua stack at stackIndex is a number.
     *
     * Exit the current function if the check failed.
     *
     * @param[in] stackIndex Index of the checked object in the stack.
     * @return The value of the object if the check succeeded.
     */
    double checkNumber(int stackIndex);

    /**
     * Pop the given number of elements from the stack.
     *
     * @param[in] n The number of elements to pop from the stack.
     */
    void pop(int n);

    /**
     * Open the base Lua library.
     *
     * This can push new elements on the stack.
     */
    void open_base();

    /**
     * Get the index of the highest element in the Lua stack.
     *
     * This is also the number of elements on the stack (elements are indexed from 1).
     *
     * @return The index of the element on the top of the stack.
     */
    int getTop();
};

#endif /* LUASTATEVIEW_HPP */

