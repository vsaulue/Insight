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

#ifndef LUADEFAULTBINDING_HPP
#define LUADEFAULTBINDING_HPP

#include <type_traits>
#include <utility>

#include "LuaBinding.hpp"
#include "LuaDefaultCall.hpp"
#include "LuaDefaultDelete.hpp"
#include "LuaStateView.hpp"
#include "LuaWrapFunction.hpp"

/**
 * Provides default implementations of some functions of LuaBinding.
 *
 * LuaBinding<BindedType> can be derived from this object. In this case, the
 * BindedType specialization must provide the following static method:
 * <code>
 *     static const std::string& luaClassName();
 * </code>
 */
template<typename BindedType>
class LuaDefaultBinding {
protected:
    /**
     * Enables a template if a type is copy constructible.
     *
     * @tparam T Type checked for a copy constructor.
     * @tparam ReturnType Type set at enable_if_copy_constructible<T>::type if the condition is true.
     */
    template<typename T, typename ReturnType=T>
    using enable_if_copy_constructible = typename std::enable_if<std::is_copy_constructible<T>::value,ReturnType>;
private:
    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state) {
        const std::string& className = LuaBinding<BindedType>::luaClassName();
        bool newTable = state.newMetatable(className);
        if (newTable) {
            using DefaultDelete = LuaDefaultDelete<BindedType>;
            if (DefaultDelete::hasDeletor) {
                state.push<int(*)(lua_State*)>(luaWrapFunction<DefaultDelete::luaDelete>);
                state.setField(-2, "__gc");
            }

            using DefaultCall = LuaDefaultCall<BindedType>;
            if (DefaultCall::hasCall) {
                state.push<int(*)(lua_State*)>(luaWrapFunction<DefaultCall::luaCall>);
                state.setField(-2,"__call");
            }
        }
    }
public:
    /**
     * Pushes a new object of type BindedType into Lua.
     *
     * @tparam ArgsType Types of the constructor arguments for type BindedType.
     *
     * @param state Lua state in which the push is done.
     * @param constructorArgs Arguments for the constructor of type BindedType.
     */
    template<typename... ArgTypes>
    static void push(LuaStateView& state, ArgTypes&&... constructorArgs) {
        state.newObject<BindedType>(std::forward<ArgTypes>(constructorArgs)...);
        pushMetatable(state);
        state.setMetatable(-2);
    }

    /**
     * Get a reference to an object of type BindedType at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired BindedType object, if the object in the stack is of this type.
     */
    static BindedType& getRef(LuaStateView& state, int stackIndex) {
        BindedType* result = state.checkUserData<BindedType>(stackIndex, LuaBinding<BindedType>::luaClassName());
        return *result;
    }

    /**
     * Gets a copy of the object of type T at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return A copy of the object at the given index, if it is of type T.
     */
    template<typename T=BindedType>
    static typename enable_if_copy_constructible<T>::type get(LuaStateView& state, int stackIndex) {
        return getRef(state, stackIndex);
    }
};


#endif /* LUADEFAULTBINDING_HPP */

