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

#ifndef LUATABLE_HPP
#define LUATABLE_HPP

#include "lua/LuaBinding.hpp"
#include "lua/LuaException.hpp"
#include "lua/LuaIsStackAlias.hpp"
#include "lua/LuaStateView.hpp"

/**
 * Pointer to a native Lua table in a Lua stack.
 *
 * This object acquires ownership of the stack slot. The destructor will pop the
 * table if the slot is at the top of the stack.
 */
class LuaTable {
public:
    /**
     * Creates a new empty table on the top of a Lua stack.
     *
     * @param state Lua stack in which the table will be created.
     */
    LuaTable(LuaStateView& state);

    /**
     * Points to a table already existing on a Lua stack.
     *
     * @param state State containing the Lua table.
     * @param stackIndex Index of the Lua table in the stack;
     */
    LuaTable(LuaStateView& state, int stackIndex);

    LuaTable(const LuaTable& table) = delete;

    /**
     * Move constructor.
     *
     * Transfers ownership of the item slot on the stack containing the table.
     *
     * @param table Moved table.
     */
    LuaTable(LuaTable&& table);

    ~LuaTable();

    /**
     * Tests if a non Nil value is present in the table at the given index.
     * @param key Entry in the table to check.
     * @return True if a non Nil value is associated with key in the table.
     * @tparam KeyType Type of the key used for the lookup.
     */
    template<typename KeyType>
    bool has(const KeyType& key) {
        state.push<KeyType>(key);
        state.getField(stackIndex);
        bool result = !state.isNil(-1);
        state.pop(1);
        return result;
    }

    /**
     * Reads a value from the Lua table.
     *
     * @param[in] key Key of the desired value in the table.
     * @return A copy of the value in the table.
     *
     * @tparam KeyType Type of the key used for the lookup.
     * @tparam ValueType Expected type to retrieve from the Lua table. Must be copy constructible.
     */
    template<typename KeyType, typename ValueType>
    ValueType get(const KeyType& key) {
        state.push<KeyType>(key);
        state.getField(stackIndex);
        ValueType result = state.get<ValueType>(state.getTop());
        if constexpr (!LuaIsStackAlias<ValueType>::value) {
            state.pop(1);
        }
        return result;
    }

    /**
     * Gets a value from the Lua table by reference.
     *
     * @param[in] key Key of the desired value in the table.
     * @return A reference to the value stored in the table.
     *
     * @tparam KeyType Type of the key used for the lookup.
     * @tparam ValueType Expected type to retrieve from the Lua table.
     */
    template<typename KeyType, typename ValueType>
    ValueType& getRef(const KeyType& key) {
        state.push<KeyType>(key);
        state.getField(stackIndex);
        ValueType& result = state.getRef<ValueType>(state.getTop());
        state.pop(1);
        return result;
    }

    /**
     * Sets a value in the Lua table.
     *
     * @param key Key of the element to set in the table.
     * @param cArgs Arguments of the constructor of the new value.
     *
     * @tparam KeyType Type of the key used to index the new element.
     * @tparam ValueType Type of the value to insert into the table.
     * @tparam ConstructorArgs Types of the arguments of ValueType constructor.
     */
    template<typename KeyType, typename ValueType, typename... ConstructorArgs>
    void set(const KeyType& key, ConstructorArgs&&... cArgs) {
        state.push<KeyType>(key);
        state.push<ValueType>(std::forward<ConstructorArgs>(cArgs)...);
        state.setField(stackIndex);
    }

    /**
     * Gets the index of this table on the Lua stack.
     * @return The stack index of this table.
     */
    int getStackIndex() const {
        return stackIndex;
    }

    /**
     * Gets the Lua state containing this table in its stack.
     * @return The Lua state holding this Lua table.
     */
    const LuaStateView& getState() const {
        return state;
    }
private:
    /** Absolute index of the table in the Lua stack (or 0 for dangling pointer). */
    int stackIndex;
    /** Lua state containing the table.*/
    LuaStateView& state;
};

/** See LuaBinding in LuaBinding.hpp. */
template<>
class LuaBinding<LuaTable> {
public:
    static constexpr bool isStackAlias = true;

    static void push(LuaStateView& state) {
        state.newTable();
    }

    static void push(LuaStateView& state, const LuaTable& table) {
        if (&table.getState() != &state) {
            std::string msg = "Cannot use a table created in another Lua state.";
            throw LuaException(msg.c_str());
        }
        state.pushValue(table.getStackIndex());
    }

    static LuaTable get(LuaStateView& state, int stackIndex) {
        return LuaTable(state, stackIndex);
    }
};

#endif /* LUATABLE_HPP */
