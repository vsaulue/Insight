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
#include <type_traits>

#include "LuaBinding.hpp"

struct lua_State;

/**
 * Wraps an existing C lua_State.
 */
class LuaStateView {
protected:
    template<typename,typename> friend class LuaBinding;
    template<typename> friend class LuaDefaultBinding;
    template<typename,typename> friend class LuaPointerBinding;

    /** Function type used by the C api of Lua. */
    using CFunction = int(*)(lua_State*);

    /** lua_State from the C API. */
    lua_State* state;

    /**
     * Creates a new Lua userdata of the specified size.
     *
     * @param[in] size Size of the memory block.
     *
     * @return A pointer to the new memory block.
     */
    void* newUserData(std::size_t size);

    /**
     * Gets an object from the stack it it is an Lua Userdata.
     *
     * @param[in] stackIndex Index of the item to get on the stack.
     * @param[in] metatableName Expected name of its metatable.
     *
     * @return A pointer to the memory block of the Userdata.
     */
    void* checkUData(int stackIndex, const std::string& metatableName);

    /**
     * Gets an object from the stack if it is a Userdata of the specified type.
     *
     * @tparam UserDataType Expected type of the object in the Lua userdata.
     *
     * @param[in] stackIndex Index of the item to get on the stack.
     * @param[in] metatableName Expected name of its metatable.
     *
     * @return A pointer to the object on the stack.
     */
    template<typename UserDataType>
    UserDataType* checkUserData(int stackIndex, const std::string& metatableName) {
        void *result = checkUData(stackIndex, metatableName);
        return reinterpret_cast<UserDataType*>(result);
    }

    /**
     * Gets an object from the stack if it is a Userdata.
     *
     * @param[in] stackIndex Index of the item to get on the stack.
     *
     * @return A pointer to the object stored in the Userdata.
     */
    void* checkUserData(int stackIndex);

    /**
     * Pushes a boolean on the Lua stack.
     *
     * @param[in] value Value to push on the stack.
     */
    void pushBool(bool value);

    /**
     * Gets a boolean in the Lua stack.
     *
     * @param[in] stackIndex Index of the boolean in the stack.
     */
    bool getBool(int stackIndex);

    /**
     * Pushes a function on the stack.
     *
     * @param[in] function function to push on the stack.
     */
    void pushCFunction(int (*function)(lua_State*));

    /**
     * Gets a C function in the Lua stack.
     *
     * @param[in] stackIndex Index of the boolean in the stack.
     * @return A pointer to the function stored at stackIndex.
     */
    CFunction getCFunction(int stackIndex);

    /**
     * Pushes the specified field of the metatable of the object at the specified index.
     *
     * @param[in] stackIndex Index of the object in the stack.
     * @param[in] fieldName Name of the field in the metatable.
     * @return true if the field was found. false if the object does not have a metatable,
     * or does not have a field named fieldName.
     */
    bool pushMetafield(int stackIndex, const std::string& fieldName);

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
        return result;
    }
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
     * Creates a new object on top of the Lua stack.
     *
     * This function uses LuaBinding<UserDataType> to push the object with binding
     * informations (metatable with methods, destructor, ...).
     *
     * @tparam UserDataType Type of the object to create inside Lua.
     * @tparam ArgsType Type of the argument of UserDataType constructor.
     *
     * @param constructorArgs Arguments of the constructor of UserDataType.
     */
    template<typename UserDataType, typename... ArgsType>
    void push(ArgsType&&... constructorArgs) {
        static_assert(std::is_constructible<UserDataType, ArgsType...>::value, "Cannot construct an object of type Userdata with the given arguments.");
        LuaBinding<UserDataType>::push(*this, std::forward<ArgsType>(constructorArgs)...);
    }

    /**
     * Gets an object of type Userdata at the given index in the Lua stack.
     *
     * This function will throw an exception if the object at the given index is
     * not of type UserDataType.
     *
     * @tparam UserDataType Expected type of the object in the stack.
     *
     * @param[in] stackIndex Index of the desired object in the Lua stack.
     * @return A copy of the object at the given index.
     */
    template<typename UserDataType>
    UserDataType get(int stackIndex) {
        static_assert(std::is_copy_constructible<UserDataType>::value, "This type cannot be returned by value.");
        return LuaBinding<UserDataType>::get(*this, stackIndex);
    }

    /**
     * Gets a reference to an object of type Userdata at the given index in the Lua stack.
     *
     * The basic types of Lua (CFunctions, bool, int, ...) cannot be returned by reference.
     *
     * This function will throw an exception if the object at the given index is
     * not of type UserDataType.
     *
     * @param[in] stackIndex Index of the desired object in the Lua stack.
     * @return A reference to the object at the given index.
     */
    template<typename UserDataType>
    UserDataType& getRef(int stackIndex) {
        return LuaBinding<UserDataType>::getRef(*this, stackIndex);
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

    /**
     * Removes the element on the stack at the given index.
     *
     * @param stackIndex Index of the element to remove.
     */
    void remove(int stackIndex);

    /**
     * Throws an Lua error.
     *
     * @param[in] msg An error message.
     */
    void throwError(const std::string& msg);

    /**
     * Throws an Lua error when an argument on the Lua stack is invalid.
     *
     * @param[in] stackIndex Index of the invalid argument.
     * @param[in] msg An error message.
     */
    void throwArgError(int stackIndex, const std::string& msg);
};

// Bindings for some basic types.
// C++ types that have a corresponding built-in type in Lua.
#include "LuaBaseBindings.hpp"
// C++ types stored as Lua Userdata.
#include "LuaFunctionBindings.hpp"
#include "LuaBindingVirtualClass.hpp"
#include "LuaPointerBindings.hpp"

#endif /* LUASTATEVIEW_HPP */

