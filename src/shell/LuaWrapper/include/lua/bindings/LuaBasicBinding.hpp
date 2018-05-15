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

#ifndef LUABASICBINDING_HPP
#define LUABASICBINDING_HPP

#include <type_traits>
#include <utility>

#include "lua/LuaBinding.hpp"
#include "lua/LuaStateView.hpp"

#include "lua/types/LuaCFunction.hpp"

#include "lua/bindings/helpers/LuaDefaultCall.hpp"
#include "lua/bindings/helpers/LuaDefaultClassName.hpp"
#include "lua/bindings/helpers/LuaDefaultDelete.hpp"
#include "lua/bindings/helpers/LuaDefaultGet.hpp"
#include "lua/bindings/helpers/LuaDefaultIndex.hpp"
#include "lua/bindings/helpers/LuaDefaultToString.hpp"
#include "lua/bindings/helpers/LuaWrapFunction.hpp"

/**
 * Provides default implementations of some functions of LuaBinding.
 *
 * This will define the following functions for Luabinding:
 * - push
 * - getRef
 * - get (if BoundType is copy constructible, or constructible from an Lua table).
 *
 * LuaBinding<BoundType> can be derived from this object.
 *
 * Optional static methods:
 * <ul>
 *   <li>
 *     <code>
 *       static int luaIndexImpl(BoundType& object, const std::string& memberName, LuaStateView& state);
 *     </code>
 *     <p> Gets the field/method of object named memberName (see lua metamethod __index).</p>
 *   </li>
 *   <li>
 *     <code>
 *       static std::string luaToStringImpl(BoundType& object);
 *     </code>
 *     <p>Implementation of Lua metamethod __tostring.</p>
 *   </li>
 *   <li>
 *     <code>
 *       static const std::string& luaClassName();<br>
 *     </code>
 *     <p>Overrides the auto-generated Lua class name of this type. Must be unique.</p>
 *   </li>
 *   <li>
 *     <code>
 *       static BoundType getFromTable(LuaTable& table);
 *     </code>
 *     <p>Implements a constructor from a Lua native table.</p>
 *   </li>
 * </ul>
 */
template<typename BoundType>
class LuaBasicBinding : public LuaDefaultClassName<BoundType> {
protected:
    static void setMetafields(LuaStateView& state) {
        using DefaultDelete = LuaDefaultDelete<BoundType>;
        if constexpr (DefaultDelete::hasDeletor) {
            state.push<LuaCFunction>(luaWrapFunction<DefaultDelete::luaDelete>);
            state.setField(-2, "__gc");
        }

        using DefaultCall = LuaDefaultCall<BoundType>;
        if constexpr (DefaultCall::hasCall) {
            state.push<LuaCFunction>(luaWrapFunction<DefaultCall::luaCall>);
            state.setField(-2, "__call");
        }

        using DefaultIndex = LuaDefaultIndex<BoundType>;
        if constexpr (DefaultIndex::hasIndex) {
            state.push<LuaCFunction>(luaWrapFunction<DefaultIndex::luaIndex>);
            state.setField(-2, "__index");
        }

        using DefaultToString = LuaDefaultToString<BoundType>;
        if constexpr (DefaultToString::hasToStringFunction) {
            state.push<LuaCFunction>(luaWrapFunction<DefaultToString::luaToString>);
            state.setField(-2, "__tostring");
        }
    }
private:
    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state) {
        const std::string& className = LuaBinding<BoundType>::luaClassName();
        bool newTable = state.newMetatable(className);
        if (newTable) {
            setMetafields(state);
        }
    }
public:
    /**
     * Pushes a new object of type BoundType into Lua.
     *
     * @tparam ArgsType Types of the constructor arguments for type BoundType.
     *
     * @param state Lua state in which the push is done.
     * @param constructorArgs Arguments for the constructor of type BoundType.
     */
    template<typename... ArgTypes>
    static void push(LuaStateView& state, ArgTypes&&... constructorArgs) {
        state.newObject<BoundType>(std::forward<ArgTypes>(constructorArgs)...);
        pushMetatable(state);
        state.setMetatable(-2);
    }

    /**
     * Get a reference to an object of type BoundType at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired BoundType object, if the object in the stack is of this type.
     */
    static BoundType& getRef(LuaStateView& state, int stackIndex) {
        BoundType* result = state.checkUserData<BoundType>(stackIndex, LuaBinding<BoundType>::luaClassName());
        return *result;
    }

    /**
     * Constructs a new C++ object of type T from an object in the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return A copy of the object at the given index, if compatible.
     */
    template<typename T=BoundType>
    static T get(LuaStateView& state, int stackIndex) {
        return LuaDefaultGet<T>::get(state, stackIndex);
    }
};

#endif /* LUABASICBINDING_HPP */

